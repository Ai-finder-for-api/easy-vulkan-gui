// 2x2 and 3x3 matrices (column-major, GLM layout): products, transpose, determinant, inverse, quaternion -> 3x3.
#pragma once
#include "scalar.hpp"
#include "vec.hpp"
#include "glsl.hpp"
#include "quat.hpp"
#include "mat4.hpp"

namespace evgx { namespace math {

struct mat2 {
    vec2<float> c[2];
    constexpr mat2() : c{vec2<float>(1, 0), vec2<float>(0, 1)} {}
    constexpr explicit mat2(float d) : c{vec2<float>(d, 0), vec2<float>(0, d)} {}
    constexpr mat2(const vec2<float>& a, const vec2<float>& b) : c{a, b} {}
    vec2<float>& operator[](int i) { return c[i]; }
    const vec2<float>& operator[](int i) const { return c[i]; }
};

struct mat3 {
    vec3<float> c[3];
    constexpr mat3() : c{vec3<float>(1, 0, 0), vec3<float>(0, 1, 0), vec3<float>(0, 0, 1)} {}
    constexpr explicit mat3(float d) : c{vec3<float>(d, 0, 0), vec3<float>(0, d, 0), vec3<float>(0, 0, d)} {}
    constexpr mat3(const vec3<float>& a, const vec3<float>& b, const vec3<float>& e) : c{a, b, e} {}
    // upper-left 3x3 of a 4x4 matrix (glm::mat3(mat4))
    explicit mat3(const mat4& m)
        : c{vec3<float>(m[0].x, m[0].y, m[0].z), vec3<float>(m[1].x, m[1].y, m[1].z), vec3<float>(m[2].x, m[2].y, m[2].z)} {}
    vec3<float>& operator[](int i) { return c[i]; }
    const vec3<float>& operator[](int i) const { return c[i]; }
};

// ---- mat2 ----
inline vec2<float> operator*(const mat2& m, const vec2<float>& v) { return m[0] * v.x + m[1] * v.y; }
inline mat2 operator*(const mat2& a, const mat2& b) { return mat2(a * b[0], a * b[1]); }
inline mat2 transpose(const mat2& m) { return mat2(vec2<float>(m[0].x, m[1].x), vec2<float>(m[0].y, m[1].y)); }
inline float determinant(const mat2& m) { return m[0].x * m[1].y - m[1].x * m[0].y; }

inline mat2 inverse(const mat2& m) {
    float d = determinant(m);
    if (abs(d) < 1e-20f) return mat2(0.f);
    float id = 1.f / d;
    return mat2(vec2<float>(m[1].y, -m[0].y) * id, vec2<float>(-m[1].x, m[0].x) * id);
}

// ---- mat3 ----
inline vec3<float> operator*(const mat3& m, const vec3<float>& v) { return m[0] * v.x + m[1] * v.y + m[2] * v.z; }
inline mat3 operator*(const mat3& a, const mat3& b) { return mat3(a * b[0], a * b[1], a * b[2]); }

inline mat3 transpose(const mat3& m) {
    return mat3(vec3<float>(m[0].x, m[1].x, m[2].x), vec3<float>(m[0].y, m[1].y, m[2].y), vec3<float>(m[0].z, m[1].z, m[2].z));
}

inline float determinant(const mat3& m) { return dot(m[0], cross(m[1], m[2])); }

// inverse through cofactors (rows of the adjugate, divided by the determinant)
inline mat3 inverse(const mat3& m) {
    vec3<float> r0 = cross(m[1], m[2]), r1 = cross(m[2], m[0]), r2 = cross(m[0], m[1]);
    float d = dot(m[0], r0);
    if (abs(d) < 1e-20f) return mat3(0.f);
    float id = 1.f / d;
    return mat3(vec3<float>(r0.x, r1.x, r2.x) * id, vec3<float>(r0.y, r1.y, r2.y) * id, vec3<float>(r0.z, r1.z, r2.z) * id);
}

inline mat3 mat3_cast(const quat& q) { return mat3(mat4_cast(q)); }

// ---- raw data access ----
inline const float* value_ptr(const mat3& m) { return &m.c[0].x; }
inline const float* value_ptr(const mat2& m) { return &m.c[0].x; }
inline const float* value_ptr(const vec2<float>& v) { return &v.x; }

}}  // namespace evgx::math
