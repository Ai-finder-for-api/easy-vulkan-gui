// Scalar math without <cmath>: constants, sqrt, sin/cos/tan, atan family, angle conversion, min/max/clamp/mix.
#pragma once
#include <stdint.h>

namespace evgx { namespace math {

constexpr float pi = 3.14159265358979323846f;

inline float abs(float x) { return x < 0 ? -x : x; }

// floor for |x| < 2^23; larger values are already integral, NaN passes through (no UB casts)
inline float floor(float x) {
    if (!(x > -8388608.f && x < 8388608.f)) return x;
    int i = (int)x;
    return (float)(i - (x < (float)i));
}

// square root by bit-trick initial guess and four Newton steps (|rel error| ~1e-7)
inline float sqrt(float x) {
    if (!(x > 0)) return 0;
    union { float f; uint32_t i; } u;
    u.f = x;
    u.i = 0x1fbd1df5u + (u.i >> 1);
    float y = u.f;
    for (int k = 0; k < 4; k++) y = 0.5f * (y + x / y);
    return y;
}

inline double sqrt(double x) {
    if (!(x > 0)) return 0;
    double y = (double)sqrt((float)x);
    for (int k = 0; k < 3; k++) y = 0.5 * (y + x / y);
    return y;
}

namespace detail {
// Cody-Waite reduction: 2*pi = 6.28125 (exact in float) + 0.0019353071795864769
inline float reduce(float x) {
    float k = floor(x * 0.15915494309189535f + 0.5f);
    return (x - k * 6.28125f) - k * 0.0019353071795864769f;
}
// odd polynomial for sin, valid for |x| <= pi/2
inline float sin_poly(float x) {
    float x2 = x * x;
    return x * (1.f + x2 * (-1.f / 6 + x2 * (1.f / 120 + x2 * (-1.f / 5040 + x2 * (1.f / 362880 + x2 * (-1.f / 39916800.f))))));
}
}

inline float sin(float x) {
    x = detail::reduce(x);
    if (x > pi / 2) x = pi - x;
    else if (x < -pi / 2) x = -pi - x;
    return detail::sin_poly(x);
}

inline float cos(float x) {
    x = abs(detail::reduce(x));                 // [0, pi]
    return detail::sin_poly(pi / 2 - x);        // cos(x) = sin(pi/2 - x)
}

inline float tan(float x) { return sin(x) / cos(x); }

// arctangent, |error| < ~1e-5 rad
inline float atan(float x) {
    float a = abs(x);
    bool inv = a > 1.f;
    if (inv) a = 1.f / a;
    float a2 = a * a;
    float r = a * (0.9998660f + a2 * (-0.3302995f + a2 * (0.1801410f + a2 * (-0.0851330f + a2 * 0.0208351f))));
    if (inv) r = pi / 2 - r;
    return x < 0 ? -r : r;
}

inline float atan2(float y, float x) {
    if (x > 0) return atan(y / x);
    if (x < 0) return y >= 0 ? atan(y / x) + pi : atan(y / x) - pi;
    return y > 0 ? pi / 2 : (y < 0 ? -pi / 2 : 0.f);
}

// arcsine and arccosine via atan (|error| ~1e-5)
inline float asin(float x) {
    x = x < -1.f ? -1.f : (x > 1.f ? 1.f : x);
    return atan(x / sqrt(1.f - x * x + 1e-30f));
}
inline float acos(float x) { return pi / 2 - asin(x); }

template <class T> constexpr T radians(T d) { return d * (T)0.017453292519943295; }
template <class T> constexpr T degrees(T r) { return r * (T)57.29577951308232; }
template <class T> constexpr T min(T a, T b) { return b < a ? b : a; }
template <class T> constexpr T max(T a, T b) { return a < b ? b : a; }
template <class T> constexpr T clamp(T v, T lo, T hi) { return v < lo ? lo : (hi < v ? hi : v); }
template <class T> constexpr T mix(T a, T b, T t) { return a + (b - a) * t; }
// mix for any vector type with operator+, operator-, operator* by a float
template <class V> constexpr V mix_v(const V& a, const V& b, float t) { return a + (b - a) * t; }

}}  // namespace evgx::math
