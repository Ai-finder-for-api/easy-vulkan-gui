#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Vec2                                                   ║
// ║  2-component float vector. Zero external deps.                  ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cmath>
#include <string>

namespace vgui {

struct Vec2 {
    float x = 0.0f, y = 0.0f;

    constexpr Vec2() = default;
    constexpr Vec2(float x, float y) : x(x), y(y) {}
    explicit constexpr Vec2(float v)  : x(v), y(v) {}

    // ── Arithmetic ────────────────────────────────────────
    constexpr Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    constexpr Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
    constexpr Vec2 operator*(float s)       const { return {x*s,   y*s  }; }
    constexpr Vec2 operator/(float s)       const { return {x/s,   y/s  }; }
    constexpr Vec2& operator+=(const Vec2& o) { x+=o.x; y+=o.y; return *this; }
    constexpr Vec2& operator-=(const Vec2& o) { x-=o.x; y-=o.y; return *this; }
    constexpr Vec2& operator*=(float s)       { x*=s;   y*=s;   return *this; }

    // ── Comparison ────────────────────────────────────────
    constexpr bool operator==(const Vec2& o) const { return x==o.x && y==o.y; }
    constexpr bool operator!=(const Vec2& o) const { return !(*this==o); }

    // ── Length ────────────────────────────────────────────
    float  length()    const { return std::sqrt(x*x + y*y); }
    float  length_sq() const { return x*x + y*y; }
    Vec2   normalized()const { float l = length(); return l > 0 ? (*this/l) : Vec2{}; }
    void   normalize()       { *this = normalized(); }

    // ── Products ──────────────────────────────────────────
    static constexpr float dot  (const Vec2& a, const Vec2& b) { return a.x*b.x + a.y*b.y; }
    static constexpr float cross(const Vec2& a, const Vec2& b) { return a.x*b.y - a.y*b.x; }

    // ── Swizzle helpers ───────────────────────────────────
    constexpr Vec2 yx() const { return {y, x}; }

    // ── Common constants ──────────────────────────────────
    static constexpr Vec2 zero ()  { return {0,0}; }
    static constexpr Vec2 one  ()  { return {1,1}; }
    static constexpr Vec2 up   ()  { return {0,1}; }
    static constexpr Vec2 right()  { return {1,0}; }

    std::string to_string() const;
};

inline constexpr Vec2 operator*(float s, const Vec2& v) { return v * s; }

} // namespace vgui
