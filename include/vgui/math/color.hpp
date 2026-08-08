#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Color                                                  ║
// ║  RGBA colour in normalised [0..1] float range.                  ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    Color red   = Color::red();                                  ║
// ║    Color custom{ 0.2f, 0.4f, 0.8f, 1.0f };                     ║
// ║    Color hex   = Color::from_hex(0xFF6030FF);                   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <string>

namespace vgui {

struct Color {
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

    constexpr Color() = default;
    constexpr Color(float r, float g, float b, float a = 1.0f)
        : r(r), g(g), b(b), a(a) {}

    // ── Factory helpers ───────────────────────────────────
    /// Construct from 0xRRGGBBAA packed uint32
    static constexpr Color from_hex(uint32_t hex) {
        return {
            ((hex >> 24) & 0xFF) / 255.0f,
            ((hex >> 16) & 0xFF) / 255.0f,
            ((hex >>  8) & 0xFF) / 255.0f,
            ((hex      ) & 0xFF) / 255.0f
        };
    }

    /// Construct from 0-255 byte values
    static constexpr Color from_bytes(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return { r/255.0f, g/255.0f, b/255.0f, a/255.0f };
    }

    // ── Pack ──────────────────────────────────────────────
    constexpr uint32_t to_hex() const {
        auto ub = [](float v) -> uint32_t { return static_cast<uint32_t>(v * 255.0f) & 0xFF; };
        return (ub(r) << 24) | (ub(g) << 16) | (ub(b) << 8) | ub(a);
    }

    // ── Blend ─────────────────────────────────────────────
    constexpr Color lerp(const Color& to, float t) const {
        return { r+(to.r-r)*t, g+(to.g-g)*t, b+(to.b-b)*t, a+(to.a-a)*t };
    }

    constexpr Color with_alpha(float new_a) const { return {r,g,b,new_a}; }

    constexpr bool operator==(const Color& o) const { return r==o.r&&g==o.g&&b==o.b&&a==o.a; }
    constexpr bool operator!=(const Color& o) const { return !(*this==o); }

    // ── Named palette ─────────────────────────────────────
    static constexpr Color black()       { return {0.0f,  0.0f,  0.0f,  1.0f}; }
    static constexpr Color white()       { return {1.0f,  1.0f,  1.0f,  1.0f}; }
    static constexpr Color red()         { return {1.0f,  0.0f,  0.0f,  1.0f}; }
    static constexpr Color green()       { return {0.0f,  1.0f,  0.0f,  1.0f}; }
    static constexpr Color blue()        { return {0.0f,  0.0f,  1.0f,  1.0f}; }
    static constexpr Color yellow()      { return {1.0f,  1.0f,  0.0f,  1.0f}; }
    static constexpr Color cyan()        { return {0.0f,  1.0f,  1.0f,  1.0f}; }
    static constexpr Color magenta()     { return {1.0f,  0.0f,  1.0f,  1.0f}; }
    static constexpr Color transparent() { return {0.0f,  0.0f,  0.0f,  0.0f}; }
    static constexpr Color gray()        { return {0.5f,  0.5f,  0.5f,  1.0f}; }
    static constexpr Color dark_gray()   { return {0.2f,  0.2f,  0.2f,  1.0f}; }
    static constexpr Color light_gray()  { return {0.75f, 0.75f, 0.75f, 1.0f}; }
    static constexpr Color orange()      { return {1.0f,  0.65f, 0.0f,  1.0f}; }
    static constexpr Color purple()      { return {0.5f,  0.0f,  0.5f,  1.0f}; }
    static constexpr Color pink()        { return {1.0f,  0.41f, 0.71f, 1.0f}; }

    std::string to_string() const;
};

} // namespace vgui
