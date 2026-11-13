// 4x4 matrices (column-major, GLM layout): products, transforms, projections, lookAt, general inverse, quaternion -> matrix.
#pragma once
#include "scalar.hpp"
#include "vec.hpp"
#include "glsl.hpp"
#include "quat.hpp"

namespace evgx { namespace math {

struct mat4 {
    vec4<float> c[4];                       // columns: m[col][row]
    constexpr mat4() : c{vec4<float>(1, 0, 0, 0), vec4<float>(0, 1, 0, 0), vec4<float>(0, 0, 1, 0), vec4<float>(0, 0, 0, 1)} {}
    constexpr explicit mat4(float d) : c{vec4<float>(d, 0, 0, 0), vec4<float>(0, d, 0, 0), vec4<float>(0, 0, d, 0), vec4<float>(0, 0, 0, d)} {}
    vec4<float>& operator[](int i) { return c[i]; }
    const vec4<float>& operator[](int i) const { return c[i]; }
};

// ---- products ----
inline vec4<float> operator*(const mat4& m, const vec4<float>& v) {
    return m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.w;
}

inline mat4 operator*(const mat4& a, const mat4& b) {
    mat4 r(0.f);
    for (int i = 0; i < 4; i++) r[i] = a[0] * b[i].x + a[1] * b[i].y + a[2] * b[i].z + a[3] * b[i].w;
    return r;
}

inline mat4 transpose(const mat4& m) {
    mat4 r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) r[i][j] = m[j][i];
    return r;
}

// ---- transforms (applied on top of m, like glm::translate(m, v)) ----
inline mat4 translate(const mat4& m, const vec3<float>& v) {
    mat4 r = m;
    r[3] = m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3];
    return r;
}

inline mat4 scale(const mat4& m, const vec3<float>& v) {
    mat4 r = m;
    r[0] = m[0] * v.x;
    r[1] = m[1] * v.y;
    r[2] = m[2] * v.z;
    return r;
}

// rotation by `angle` radians about `axis` (Rodrigues' formula)
inline mat4 rotate(const mat4& m, float angle, const vec3<float>& axis_in) {
    float c = cos(angle), s = sin(angle);
    vec3<float> a = normalize(axis_in), t = a * (1.f - c);
    mat4 R(0.f);
    R[0][0] = c + t.x * a.x;        R[0][1] = t.x * a.y + s * a.z;  R[0][2] = t.x * a.z - s * a.y;
    R[1][0] = t.y * a.x - s * a.z;  R[1][1] = c + t.y * a.y;        R[1][2] = t.y * a.z + s * a.x;
    R[2][0] = t.z * a.x + s * a.y;  R[2][1] = t.z * a.y - s * a.x;  R[2][2] = c + t.z * a.z;
    mat4 r;
    r[0] = m[0] * R[0][0] + m[1] * R[0][1] + m[2] * R[0][2];
    r[1] = m[0] * R[1][0] + m[1] * R[1][1] + m[2] * R[1][2];
    r[2] = m[0] * R[2][0] + m[1] * R[2][1] + m[2] * R[2][2];
    r[3] = m[3];
    return r;
}

// ---- projections ----
// GLM default: right-handed, clip z in [-1, 1] (OpenGL)
inline mat4 perspective(float fovy, float aspect, float zn, float zf) {
    float t = tan(fovy / 2);
    mat4 r(0.f);
    r[0][0] = 1.f / (aspect * t);
    r[1][1] = 1.f / t;
    r[2][2] = -(zf + zn) / (zf - zn);
    r[2][3] = -1.f;
    r[3][2] = -(2.f * zf * zn) / (zf - zn);
    return r;
}

// Vulkan clip space: z in [0, 1], Y points down
inline mat4 perspective_vk(float fovy, float aspect, float zn, float zf) {
    float t = tan(fovy / 2);
    mat4 r(0.f);
    r[0][0] = 1.f / (aspect * t);
    r[1][1] = -1.f / t;
    r[2][2] = zf / (zn - zf);
    r[2][3] = -1.f;
    r[3][2] = -(zf * zn) / (zf - zn);
    return r;
}

inline mat4 ortho(float l, float r_, float b, float t, float n, float f) {
    mat4 r;
    r[0][0] = 2.f / (r_ - l);
    r[1][1] = 2.f / (t - b);
    r[2][2] = -2.f / (f - n);
    r[3][0] = -(r_ + l) / (r_ - l);
    r[3][1] = -(t + b) / (t - b);
    r[3][2] = -(f + n) / (f - n);
    return r;
}

// GLM's 4-argument form: near = -1, far = 1
inline mat4 ortho(float l, float r_, float b, float t) { return ortho(l, r_, b, t, -1.f, 1.f); }

inline mat4 lookAt(const vec3<float>& eye, const vec3<float>& center, const vec3<float>& up) {
    vec3<float> f = normalize(center - eye), s = normalize(cross(f, up)), u = cross(s, f);
    mat4 r;
    r[0][0] = s.x; r[1][0] = s.y; r[2][0] = s.z;
    r[0][1] = u.x; r[1][1] = u.y; r[2][1] = u.z;
    r[0][2] = -f.x; r[1][2] = -f.y; r[2][2] = -f.z;
    r[3][0] = -dot(s, eye); r[3][1] = -dot(u, eye); r[3][2] = dot(f, eye);
    return r;
}

// ---- inverse: Gauss-Jordan with partial pivoting; identity when singular ----
inline mat4 inverse(const mat4& in) {
    float a[4][8];
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++) { a[r][c] = in[c][r]; a[r][c + 4] = (r == c) ? 1.f : 0.f; }
    for (int col = 0; col < 4; col++) {
        int piv = col;
        for (int r = col + 1; r < 4; r++) if (abs(a[r][col]) > abs(a[piv][col])) piv = r;
        if (abs(a[piv][col]) < 1e-12f) return mat4();
        if (piv != col) for (int c = 0; c < 8; c++) { float t = a[col][c]; a[col][c] = a[piv][c]; a[piv][c] = t; }
        float inv = 1.f / a[col][col];
        for (int c = 0; c < 8; c++) a[col][c] *= inv;
        for (int r = 0; r < 4; r++) {
            if (r == col) continue;
            float f = a[r][col];
            if (f != 0.f) for (int c = 0; c < 8; c++) a[r][c] -= f * a[col][c];
        }
    }
    mat4 out;
    for (int r = 0; r < 4; r++) for (int c = 0; c < 4; c++) out[c][r] = a[r][c + 4];
    return out;
}

// ---- quaternion -> rotation matrix ----
inline mat4 mat4_cast(const quat& q) {
    float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z, xz = q.x * q.z, xy = q.x * q.y, yz = q.y * q.z;
    float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;
    mat4 r;
    r[0][0] = 1.f - 2.f * (yy + zz); r[0][1] = 2.f * (xy + wz);       r[0][2] = 2.f * (xz - wy);
    r[1][0] = 2.f * (xy - wz);       r[1][1] = 1.f - 2.f * (xx + zz); r[1][2] = 2.f * (yz + wx);
    r[2][0] = 2.f * (xz + wy);       r[2][1] = 2.f * (yz - wx);       r[2][2] = 1.f - 2.f * (xx + yy);
    return r;
}

// ---- raw data access ----
inline const float* value_ptr(const mat4& m) { return &m.c[0].x; }
template <class T> const T* value_ptr(const vec3<T>& v) { return &v.x; }
template <class T> const T* value_ptr(const vec4<T>& v) { return &v.x; }

}}  // namespace evgx::math
