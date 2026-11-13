// Color helpers: 8-bit RGBA packing (0xAARRGGBB), float conversion, sRGB <-> linear, HSV -> RGB, lerp.
#pragma once
#include <stdint.h>
#include <evg/math.hpp>

namespace evgx { namespace color {

inline uint32_t pack(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}
inline uint8_t red(uint32_t c) { return (uint8_t)(c >> 16); }
inline uint8_t green(uint32_t c) { return (uint8_t)(c >> 8); }
inline uint8_t blue(uint32_t c) { return (uint8_t)c; }
inline uint8_t alpha(uint32_t c) { return (uint8_t)(c >> 24); }

// float channels in [0,1] to 8 bits (clamped, rounded)
inline uint8_t to_u8(float v) {
    v = v < 0.f ? 0.f : (v > 1.f ? 1.f : v);
    return (uint8_t)(v * 255.f + 0.5f);
}
inline float to_float(uint8_t v) { return (float)v * (1.0f / 255.0f); }

// sRGB transfer curves (the exact piecewise definitions)
inline float srgb_to_linear(float c) {
    if (c <= 0.04045f) return c / 12.92f;
    return math::pow((c + 0.055f) / 1.055f, 2.4f);
}
inline float linear_to_srgb(float c) {
    if (c <= 0.0031308f) return c * 12.92f;
    return 1.055f * math::pow(c, 1.0f / 2.4f) - 0.055f;
}

// hue in degrees [0,360), saturation and value in [0,1]
inline math::vec3<float> hsv_to_rgb(float h, float s, float v) {
    h = math::mod(h, 360.f);
    if (h < 0.f) h += 360.f;
    float c = v * s;
    float hp = h / 60.f;
    float x = c * (1.f - math::abs(math::mod(hp, 2.f) - 1.f));
    float r = 0, g = 0, b = 0;
    if (hp < 1) { r = c; g = x; }
    else if (hp < 2) { r = x; g = c; }
    else if (hp < 3) { g = c; b = x; }
    else if (hp < 4) { g = x; b = c; }
    else if (hp < 5) { r = x; b = c; }
    else { r = c; b = x; }
    float m = v - c;
    return math::vec3<float>(r + m, g + m, b + m);
}

inline uint32_t lerp(uint32_t a, uint32_t b, float t) {
    auto ch = [&](int shift) {
        float x = (float)((a >> shift) & 255), y = (float)((b >> shift) & 255);
        return (uint32_t)(x + (y - x) * t + 0.5f) & 255u;
    };
    return (ch(24) << 24) | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}

}}  // namespace evgx::color
