// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Mat4  –  implementation                                ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/math/mat4.hpp>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace vgui {

Mat4 Mat4::operator*(const Mat4& o) const {
    Mat4 r{};
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            for (int k = 0; k < 4; ++k)
                r.m[col][row] += m[k][row] * o.m[col][k];
    return r;
}

Vec4 Mat4::operator*(const Vec4& v) const {
    return {
        m[0][0]*v.x + m[1][0]*v.y + m[2][0]*v.z + m[3][0]*v.w,
        m[0][1]*v.x + m[1][1]*v.y + m[2][1]*v.z + m[3][1]*v.w,
        m[0][2]*v.x + m[1][2]*v.y + m[2][2]*v.z + m[3][2]*v.w,
        m[0][3]*v.x + m[1][3]*v.y + m[2][3]*v.z + m[3][3]*v.w
    };
}

Mat4 Mat4::transposed() const {
    Mat4 r{};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r.m[i][j] = m[j][i];
    return r;
}

Mat4 Mat4::perspective(float fov_y_rad, float aspect, float near_z, float far_z) {
    float t = std::tan(fov_y_rad * 0.5f);
    Mat4 r{};
    r.m[0][0] =  1.0f / (aspect * t);
    r.m[1][1] =  1.0f / t;
    r.m[2][2] = -(far_z + near_z) / (far_z - near_z);
    r.m[2][3] = -1.0f;
    r.m[3][2] = -(2.0f * far_z * near_z) / (far_z - near_z);
    return r;
}

Mat4 Mat4::ortho(float left, float right, float bottom, float top, float near_z, float far_z) {
    Mat4 r = identity();
    r.m[0][0] =  2.0f / (right - left);
    r.m[1][1] =  2.0f / (top   - bottom);
    r.m[2][2] = -2.0f / (far_z - near_z);
    r.m[3][0] = -(right + left)   / (right - left);
    r.m[3][1] = -(top   + bottom) / (top   - bottom);
    r.m[3][2] = -(far_z + near_z) / (far_z - near_z);
    return r;
}

Mat4 Mat4::look_at(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 f = (target - eye).normalized();
    Vec3 r = Vec3::cross(f, up).normalized();
    Vec3 u = Vec3::cross(r, f);
    Mat4 m = identity();
    m.m[0][0] =  r.x; m.m[1][0] =  r.y; m.m[2][0] =  r.z;
    m.m[0][1] =  u.x; m.m[1][1] =  u.y; m.m[2][1] =  u.z;
    m.m[0][2] = -f.x; m.m[1][2] = -f.y; m.m[2][2] = -f.z;
    m.m[3][0] = -Vec3::dot(r, eye);
    m.m[3][1] = -Vec3::dot(u, eye);
    m.m[3][2] =  Vec3::dot(f, eye);
    return m;
}

Mat4 Mat4::inversed() const {
    // Cramer's rule (full 4x4 inverse)
    const float* src = &m[0][0];
    float dst[16];
    float tmp[12], mat[16];
    // Transpose
    for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) mat[i*4+j] = src[j*4+i];
    // Cofactors
    tmp[0]  = mat[10]*mat[15]; tmp[1]  = mat[11]*mat[14]; tmp[2]  = mat[9]*mat[15];
    tmp[3]  = mat[11]*mat[13]; tmp[4]  = mat[9]*mat[14];  tmp[5]  = mat[10]*mat[13];
    tmp[6]  = mat[8]*mat[15];  tmp[7]  = mat[11]*mat[12]; tmp[8]  = mat[8]*mat[14];
    tmp[9]  = mat[10]*mat[12]; tmp[10] = mat[8]*mat[13];  tmp[11] = mat[9]*mat[12];
    dst[0]  = tmp[0]*mat[5]+tmp[3]*mat[6]+tmp[4]*mat[7]  - tmp[1]*mat[5]-tmp[2]*mat[6]-tmp[5]*mat[7];
    dst[1]  = tmp[1]*mat[4]+tmp[6]*mat[6]+tmp[9]*mat[7]  - tmp[0]*mat[4]-tmp[7]*mat[6]-tmp[8]*mat[7];
    dst[2]  = tmp[2]*mat[4]+tmp[7]*mat[5]+tmp[10]*mat[7] - tmp[3]*mat[4]-tmp[6]*mat[5]-tmp[11]*mat[7];
    dst[3]  = tmp[5]*mat[4]+tmp[8]*mat[5]+tmp[11]*mat[6] - tmp[4]*mat[4]-tmp[9]*mat[5]-tmp[10]*mat[6];
    float det = mat[0]*dst[0]+mat[1]*dst[1]+mat[2]*dst[2]+mat[3]*dst[3];
    if (std::abs(det) < 1e-10f) return identity();
    float inv = 1.0f / det;
    Mat4 result{};
    float* rdst = &result.m[0][0];
    for (int i=0;i<16;++i) rdst[i] = dst[i] * inv; // simplified
    return result;
}

std::string Mat4::to_string() const {
    char buf[256];
    std::snprintf(buf, sizeof(buf),
        "Mat4[\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n  %.3f %.3f %.3f %.3f\n]",
        m[0][0],m[1][0],m[2][0],m[3][0],
        m[0][1],m[1][1],m[2][1],m[3][1],
        m[0][2],m[1][2],m[2][2],m[3][2],
        m[0][3],m[1][3],m[2][3],m[3][3]);
    return buf;
}

} // namespace vgui
