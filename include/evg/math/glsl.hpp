// GLSL-style helpers with GLM names: exp, log, pow, fract, mod, step, smoothstep, component-wise vec overloads, reflect, refract.
#pragma once
#include "scalar.hpp"
#include "vec.hpp"

namespace evgx { namespace math {

// exp via 2^k * e^r; |rel error| < 1e-6 for |x| <= 80
inline float exp(float x) {
    if (x > 88.f) return 3.4e38f;
    if (x < -87.f) return 0.f;
    float k = floor(x * 1.44269504f + 0.5f);
    if (k > 126.f) k = 126.f;
    if (k < -126.f) k = -126.f;
    float r = x - k * 0.69314718f;
    float p = 1.f + r * (1.f + r * (0.5f + r * (1.f / 6 + r * (1.f / 24 + r * (1.f / 120 + r * (1.f / 720))))));
    union { float f; uint32_t i; } u;
    u.i = (uint32_t)((int)k + 127) << 23;
    return p * u.f;
}

// natural log via mantissa/exponent split and an atanh series
inline float log(float x) {
    if (!(x > 0.f)) return x == 0.f ? -1e30f : 0.f;
    union { float f; uint32_t i; } u;
    u.f = x;
    int e = (int)((u.i >> 23) & 255) - 127;
    u.i = (u.i & 0x007FFFFFu) | 0x3F800000u;
    float m = u.f;
    if (m > 1.41421356f) { m *= 0.5f; e += 1; }
    float s = (m - 1.f) / (m + 1.f), s2 = s * s;
    float lm = 2.f * s * (1.f + s2 * (1.f / 3 + s2 * (1.f / 5 + s2 * (1.f / 7 + s2 * (1.f / 9)))));
    return (float)e * 0.69314718f + lm;
}

inline float pow(float x, float y) {
    if (x == 0.f) return y > 0.f ? 0.f : (y == 0.f ? 1.f : 1e30f);
    if (x > 0.f) return exp(y * log(x));
    float yi = floor(y);
    if (yi != y || yi > 1e9f || yi < -1e9f) return 0.f;     // negative base: integer exponents only
    float r = exp(y * log(-x));
    return ((int)yi & 1) ? -r : r;
}

inline float fract(float x) { return x - floor(x); }
inline float ceil(float x) { return -floor(-x); }
inline float mod(float x, float y) { return x - y * floor(x / y); }
inline float sign(float x) { return x > 0.f ? 1.f : (x < 0.f ? -1.f : 0.f); }
inline float step(float edge, float x) { return x < edge ? 0.f : 1.f; }

inline float smoothstep(float e0, float e1, float x) {
    float t = (x - e0) / (e1 - e0);
    t = t < 0.f ? 0.f : (t > 1.f ? 1.f : t);
    return t * t * (3.f - 2.f * t);
}

// component-wise overloads for vec2/vec3/vec4 (float; GLSL's default precision)
#define EVG_VFN1(NAME) \
    inline vec2<float> NAME(const vec2<float>& v) { return vec2<float>(NAME(v.x), NAME(v.y)); } \
    inline vec3<float> NAME(const vec3<float>& v) { return vec3<float>(NAME(v.x), NAME(v.y), NAME(v.z)); } \
    inline vec4<float> NAME(const vec4<float>& v) { return vec4<float>(NAME(v.x), NAME(v.y), NAME(v.z), NAME(v.w)); }
#define EVG_VFN2(NAME) \
    inline vec2<float> NAME(const vec2<float>& a, const vec2<float>& b) { return vec2<float>(NAME(a.x, b.x), NAME(a.y, b.y)); } \
    inline vec3<float> NAME(const vec3<float>& a, const vec3<float>& b) { return vec3<float>(NAME(a.x, b.x), NAME(a.y, b.y), NAME(a.z, b.z)); } \
    inline vec4<float> NAME(const vec4<float>& a, const vec4<float>& b) { return vec4<float>(NAME(a.x, b.x), NAME(a.y, b.y), NAME(a.z, b.z), NAME(a.w, b.w)); } \
    inline vec2<float> NAME(const vec2<float>& a, float b) { return vec2<float>(NAME(a.x, b), NAME(a.y, b)); } \
    inline vec3<float> NAME(const vec3<float>& a, float b) { return vec3<float>(NAME(a.x, b), NAME(a.y, b), NAME(a.z, b)); } \
    inline vec4<float> NAME(const vec4<float>& a, float b) { return vec4<float>(NAME(a.x, b), NAME(a.y, b), NAME(a.z, b), NAME(a.w, b)); }

EVG_VFN1(exp) EVG_VFN1(log) EVG_VFN1(abs) EVG_VFN1(floor) EVG_VFN1(ceil) EVG_VFN1(fract) EVG_VFN1(sign) EVG_VFN1(sqrt)
EVG_VFN2(pow) EVG_VFN2(mod) EVG_VFN2(min) EVG_VFN2(max)
#undef EVG_VFN1
#undef EVG_VFN2

inline vec2<float> step(float e, const vec2<float>& v) { return vec2<float>(step(e, v.x), step(e, v.y)); }
inline vec3<float> step(float e, const vec3<float>& v) { return vec3<float>(step(e, v.x), step(e, v.y), step(e, v.z)); }
inline vec4<float> step(float e, const vec4<float>& v) { return vec4<float>(step(e, v.x), step(e, v.y), step(e, v.z), step(e, v.w)); }

inline vec2<float> clamp(const vec2<float>& v, float lo, float hi) { return vec2<float>(clamp(v.x, lo, hi), clamp(v.y, lo, hi)); }
inline vec3<float> clamp(const vec3<float>& v, float lo, float hi) { return vec3<float>(clamp(v.x, lo, hi), clamp(v.y, lo, hi), clamp(v.z, lo, hi)); }
inline vec4<float> clamp(const vec4<float>& v, float lo, float hi) { return vec4<float>(clamp(v.x, lo, hi), clamp(v.y, lo, hi), clamp(v.z, lo, hi), clamp(v.w, lo, hi)); }

inline vec2<float> mix(const vec2<float>& a, const vec2<float>& b, float t) { return a + (b - a) * t; }
inline vec3<float> mix(const vec3<float>& a, const vec3<float>& b, float t) { return a + (b - a) * t; }
inline vec4<float> mix(const vec4<float>& a, const vec4<float>& b, float t) { return a + (b - a) * t; }

inline vec3<float> reflect(const vec3<float>& I, const vec3<float>& N) { return I - N * (2.f * dot(N, I)); }

inline vec3<float> refract(const vec3<float>& I, const vec3<float>& N, float eta) {
    float d = dot(N, I), k = 1.f - eta * eta * (1.f - d * d);
    if (k < 0.f) return vec3<float>(0.f, 0.f, 0.f);
    return I * eta - N * (eta * d + sqrt(k));
}

}}  // namespace evgx::math
