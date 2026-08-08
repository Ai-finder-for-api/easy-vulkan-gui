#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Vec3                                                   ║
// ║  3-component float vector.                                      ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "vec2.hpp"

namespace vgui {

struct Vec3 {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    constexpr Vec3() = default;
    constexpr Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    explicit constexpr Vec3(float v)           : x(v), y(v), z(v) {}
    constexpr Vec3(const Vec2& v, float z)     : x(v.x), y(v.y), z(z) {}

    constexpr Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    constexpr Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    constexpr Vec3 operator*(float s)       const { return {x*s,   y*s,   z*s  }; }
    constexpr Vec3 operator/(float s)       const { return {x/s,   y/s,   z/s  }; }
    constexpr Vec3 operator-()              const { return {-x, -y, -z}; }
    constexpr Vec3& operator+=(const Vec3& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
    constexpr Vec3& operator-=(const Vec3& o) { x-=o.x; y-=o.y; z-=o.z; return *this; }
    constexpr Vec3& operator*=(float s)       { x*=s;   y*=s;   z*=s;   return *this; }

    constexpr bool operator==(const Vec3& o) const { return x==o.x&&y==o.y&&z==o.z; }
    constexpr bool operator!=(const Vec3& o) const { return !(*this==o); }

    float  length()    const { return std::sqrt(x*x+y*y+z*z); }
    float  length_sq() const { return x*x+y*y+z*z; }
    Vec3   normalized()const { float l=length(); return l>0?(*this/l):Vec3{}; }
    void   normalize()       { *this = normalized(); }

    static constexpr float dot  (const Vec3& a, const Vec3& b)
        { return a.x*b.x + a.y*b.y + a.z*b.z; }
    static constexpr Vec3  cross(const Vec3& a, const Vec3& b)
        { return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x}; }

    // ── Swizzle ───────────────────────────────────────────
    constexpr Vec2 xy() const { return {x, y}; }
    constexpr Vec2 xz() const { return {x, z}; }

    static constexpr Vec3 zero   () { return {0,0,0}; }
    static constexpr Vec3 one    () { return {1,1,1}; }
    static constexpr Vec3 up     () { return {0,1,0}; }
    static constexpr Vec3 right  () { return {1,0,0}; }
    static constexpr Vec3 forward() { return {0,0,-1}; }

    std::string to_string() const;
};

inline constexpr Vec3 operator*(float s, const Vec3& v) { return v * s; }

} // namespace vgui
