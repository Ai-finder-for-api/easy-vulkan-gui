#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Mat4                                                   ║
// ║  Column-major 4x4 float matrix. Zero external deps.             ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "vec3.hpp"
#include "vec4.hpp"
#include <cmath>
#include <string>

namespace vgui {

struct Mat4 {
    // Stored column-major: m[col][row]
    float m[4][4] = {};

    constexpr Mat4() = default;

    // ── Factory ───────────────────────────────────────────
    static constexpr Mat4 identity() {
        Mat4 r; r.m[0][0]=r.m[1][1]=r.m[2][2]=r.m[3][3]=1.0f; return r;
    }

    static Mat4 translate(const Vec3& t) {
        Mat4 r = identity();
        r.m[3][0]=t.x; r.m[3][1]=t.y; r.m[3][2]=t.z;
        return r;
    }

    static Mat4 scale(const Vec3& s) {
        Mat4 r = identity();
        r.m[0][0]=s.x; r.m[1][1]=s.y; r.m[2][2]=s.z;
        return r;
    }

    static Mat4 rotate_x(float rad) {
        Mat4 r = identity();
        float c=std::cos(rad), s=std::sin(rad);
        r.m[1][1]=c; r.m[1][2]=s; r.m[2][1]=-s; r.m[2][2]=c;
        return r;
    }

    static Mat4 rotate_y(float rad) {
        Mat4 r = identity();
        float c=std::cos(rad), s=std::sin(rad);
        r.m[0][0]=c; r.m[0][2]=-s; r.m[2][0]=s; r.m[2][2]=c;
        return r;
    }

    static Mat4 rotate_z(float rad) {
        Mat4 r = identity();
        float c=std::cos(rad), s=std::sin(rad);
        r.m[0][0]=c; r.m[0][1]=s; r.m[1][0]=-s; r.m[1][1]=c;
        return r;
    }

    static Mat4 perspective(float fov_y_rad, float aspect, float near_z, float far_z);
    static Mat4 ortho(float left, float right, float bottom, float top, float near_z, float far_z);
    static Mat4 look_at(const Vec3& eye, const Vec3& target, const Vec3& up);

    // ── Multiply ──────────────────────────────────────────
    Mat4 operator*(const Mat4& o) const;
    Vec4 operator*(const Vec4& v) const;

    // ── Access ────────────────────────────────────────────
    float*       data()       { return &m[0][0]; }
    const float* data() const { return &m[0][0]; }

    Mat4 transposed() const;
    Mat4 inversed()   const;

    std::string to_string() const;
};

} // namespace vgui
