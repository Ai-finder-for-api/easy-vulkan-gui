// Vectors: vec2/vec3/vec4 (float, double, int, unsigned), component-wise operators, dot, cross, length, normalize, distance.
#pragma once
#include <stdint.h>
#include "scalar.hpp"

namespace evgx { namespace math {

template <class T> struct vec2 {
    T x, y;
    constexpr vec2() : x(0), y(0) {}
    constexpr explicit vec2(T s) : x(s), y(s) {}
    constexpr vec2(T x_, T y_) : x(x_), y(y_) {}
    T& operator[](int i) { return (&x)[i]; }
    const T& operator[](int i) const { return (&x)[i]; }
};

template <class T> struct vec3 {
    T x, y, z;
    constexpr vec3() : x(0), y(0), z(0) {}
    constexpr explicit vec3(T s) : x(s), y(s), z(s) {}
    constexpr vec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
    constexpr vec3(const vec2<T>& v, T z_) : x(v.x), y(v.y), z(z_) {}
    T& operator[](int i) { return (&x)[i]; }
    const T& operator[](int i) const { return (&x)[i]; }
};

template <class T> struct vec4 {
    T x, y, z, w;
    constexpr vec4() : x(0), y(0), z(0), w(0) {}
    constexpr explicit vec4(T s) : x(s), y(s), z(s), w(s) {}
    constexpr vec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
    constexpr vec4(const vec3<T>& v, T w_) : x(v.x), y(v.y), z(v.z), w(w_) {}
    T& operator[](int i) { return (&x)[i]; }
    const T& operator[](int i) const { return (&x)[i]; }
};

// ---- arithmetic: vector op vector, vector op scalar, scalar op vector ----
#define EVG_BIN2(op) template <class T> constexpr vec2<T> operator op(const vec2<T>& a, const vec2<T>& b) { return vec2<T>(a.x op b.x, a.y op b.y); } \
    template <class T> constexpr vec2<T> operator op(const vec2<T>& a, T s) { return vec2<T>(a.x op s, a.y op s); } \
    template <class T> constexpr vec2<T> operator op(T s, const vec2<T>& a) { return vec2<T>(s op a.x, s op a.y); }
#define EVG_BIN3(op) template <class T> constexpr vec3<T> operator op(const vec3<T>& a, const vec3<T>& b) { return vec3<T>(a.x op b.x, a.y op b.y, a.z op b.z); } \
    template <class T> constexpr vec3<T> operator op(const vec3<T>& a, T s) { return vec3<T>(a.x op s, a.y op s, a.z op s); } \
    template <class T> constexpr vec3<T> operator op(T s, const vec3<T>& a) { return vec3<T>(s op a.x, s op a.y, s op a.z); }
#define EVG_BIN4(op) template <class T> constexpr vec4<T> operator op(const vec4<T>& a, const vec4<T>& b) { return vec4<T>(a.x op b.x, a.y op b.y, a.z op b.z, a.w op b.w); } \
    template <class T> constexpr vec4<T> operator op(const vec4<T>& a, T s) { return vec4<T>(a.x op s, a.y op s, a.z op s, a.w op s); } \
    template <class T> constexpr vec4<T> operator op(T s, const vec4<T>& a) { return vec4<T>(s op a.x, s op a.y, s op a.z, s op a.w); }
EVG_BIN2(+) EVG_BIN2(-) EVG_BIN2(*) EVG_BIN2(/)
EVG_BIN3(+) EVG_BIN3(-) EVG_BIN3(*) EVG_BIN3(/)
EVG_BIN4(+) EVG_BIN4(-) EVG_BIN4(*) EVG_BIN4(/)
#undef EVG_BIN2
#undef EVG_BIN3
#undef EVG_BIN4

template <class T> constexpr vec2<T> operator-(const vec2<T>& a) { return vec2<T>(-a.x, -a.y); }
template <class T> constexpr vec3<T> operator-(const vec3<T>& a) { return vec3<T>(-a.x, -a.y, -a.z); }
template <class T> constexpr vec4<T> operator-(const vec4<T>& a) { return vec4<T>(-a.x, -a.y, -a.z, -a.w); }

template <class T> vec2<T>& operator+=(vec2<T>& a, const vec2<T>& b) { return a = a + b; }
template <class T> vec3<T>& operator+=(vec3<T>& a, const vec3<T>& b) { return a = a + b; }
template <class T> vec4<T>& operator+=(vec4<T>& a, const vec4<T>& b) { return a = a + b; }
template <class T> vec2<T>& operator-=(vec2<T>& a, const vec2<T>& b) { return a = a - b; }
template <class T> vec3<T>& operator-=(vec3<T>& a, const vec3<T>& b) { return a = a - b; }
template <class T> vec4<T>& operator-=(vec4<T>& a, const vec4<T>& b) { return a = a - b; }
template <class T> vec2<T>& operator*=(vec2<T>& a, T s) { return a = a * s; }
template <class T> vec3<T>& operator*=(vec3<T>& a, T s) { return a = a * s; }
template <class T> vec4<T>& operator*=(vec4<T>& a, T s) { return a = a * s; }

// ---- comparison ----
template <class T> constexpr bool operator==(const vec2<T>& a, const vec2<T>& b) { return a.x == b.x && a.y == b.y; }
template <class T> constexpr bool operator==(const vec3<T>& a, const vec3<T>& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
template <class T> constexpr bool operator==(const vec4<T>& a, const vec4<T>& b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
template <class T> constexpr bool operator!=(const vec2<T>& a, const vec2<T>& b) { return !(a == b); }
template <class T> constexpr bool operator!=(const vec3<T>& a, const vec3<T>& b) { return !(a == b); }
template <class T> constexpr bool operator!=(const vec4<T>& a, const vec4<T>& b) { return !(a == b); }

// ---- geometry ----
template <class T> constexpr T dot(const vec2<T>& a, const vec2<T>& b) { return a.x * b.x + a.y * b.y; }
template <class T> constexpr T dot(const vec3<T>& a, const vec3<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
template <class T> constexpr T dot(const vec4<T>& a, const vec4<T>& b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

template <class T> constexpr vec3<T> cross(const vec3<T>& a, const vec3<T>& b) {
    return vec3<T>(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y);
}

template <class V> auto length(const V& v) -> decltype(v.x) { return sqrt(dot(v, v)); }
template <class V> V normalize(const V& v) { return v * ((decltype(v.x))1 / length(v)); }
template <class V> auto distance(const V& a, const V& b) -> decltype(a.x) { return length(a - b); }

// ---- GLM-style type names ----
typedef vec2<float> vec2f;   typedef vec3<float> vec3f;   typedef vec4<float> vec4f;
typedef vec2<double> dvec2;  typedef vec3<double> dvec3;  typedef vec4<double> dvec4;
typedef vec2<int32_t> ivec2; typedef vec3<int32_t> ivec3; typedef vec4<int32_t> ivec4;
typedef vec2<uint32_t> uvec2; typedef vec3<uint32_t> uvec3; typedef vec4<uint32_t> uvec4;

}}  // namespace evgx::math
