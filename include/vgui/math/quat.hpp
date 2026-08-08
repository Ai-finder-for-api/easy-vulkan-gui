#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Quat                                                   ║
// ║  Unit quaternion for 3-D rotation.                              ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "vec3.hpp"
#include "mat4.hpp"
#include <cmath>

namespace vgui {

struct Quat {
    float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

    constexpr Quat() = default;
    constexpr Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // ── Factory ───────────────────────────────────────────
    static Quat from_axis_angle(const Vec3& axis, float rad) {
        float s = std::sin(rad * 0.5f);
        Vec3  a = axis.normalized();
        return { a.x*s, a.y*s, a.z*s, std::cos(rad * 0.5f) };
    }

    static Quat from_euler(float pitch, float yaw, float roll);
    static Quat from_mat4 (const Mat4& m);

    // ── Operations ────────────────────────────────────────
    Quat   operator*(const Quat& o) const;
    Vec3   operator*(const Vec3& v) const;
    Quat   conjugate()  const { return {-x,-y,-z, w}; }
    Quat   normalized() const;
    float  length()     const { return std::sqrt(x*x+y*y+z*z+w*w); }
    Mat4   to_mat4()    const;

    // ── Interpolation ─────────────────────────────────────
    static Quat slerp(const Quat& a, const Quat& b, float t);
    static Quat lerp (const Quat& a, const Quat& b, float t);

    static constexpr Quat identity() { return {0,0,0,1}; }
    std::string to_string() const;
};

} // namespace vgui
