#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Vec4                                                   ║
// ║  4-component float vector. Also used as RGBA colour.            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "vec3.hpp"

namespace vgui {

struct Vec4 {
    float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

    constexpr Vec4() = default;
    constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    explicit  constexpr Vec4(float v)                  : x(v), y(v), z(v), w(v) {}
    constexpr Vec4(const Vec3& v, float w)             : x(v.x), y(v.y), z(v.z), w(w) {}

    constexpr Vec4 operator+(const Vec4& o) const { return {x+o.x,y+o.y,z+o.z,w+o.w}; }
    constexpr Vec4 operator-(const Vec4& o) const { return {x-o.x,y-o.y,z-o.z,w-o.w}; }
    constexpr Vec4 operator*(float s)       const { return {x*s,  y*s,  z*s,  w*s  }; }
    constexpr Vec4 operator/(float s)       const { return {x/s,  y/s,  z/s,  w/s  }; }

    constexpr bool operator==(const Vec4& o) const { return x==o.x&&y==o.y&&z==o.z&&w==o.w; }
    constexpr bool operator!=(const Vec4& o) const { return !(*this==o); }

    float  length()     const { return std::sqrt(x*x+y*y+z*z+w*w); }
    Vec4   normalized() const { float l=length(); return l>0?(*this/l):Vec4{}; }

    static constexpr float dot(const Vec4& a, const Vec4& b)
        { return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w; }

    // ── Swizzle ───────────────────────────────────────────
    constexpr Vec2 xy()  const { return {x, y}; }
    constexpr Vec3 xyz() const { return {x, y, z}; }

    static constexpr Vec4 zero() { return {0,0,0,0}; }
    static constexpr Vec4 one()  { return {1,1,1,1}; }

    std::string to_string() const;
};

} // namespace vgui
