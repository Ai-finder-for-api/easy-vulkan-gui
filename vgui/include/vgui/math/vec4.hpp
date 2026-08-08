#pragma once

#include <cmath>
#include <ostream>
#include "vgui/math/vec3.hpp"

namespace vgui {

/**
 * @brief 4D vector with basic operations
 */
template<typename T>
struct Vec4 {
    T x, y, z, w;
    
    // Constructors
    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    
    // Conversion from Vec3
    Vec4(T x, T y, T z) : x(x), y(y), z(z), w(1) {}
    
    // Conversion constructor
    template<typename U>
    Vec4(const Vec4<U>& other) : x(static_cast<T>(other.x)), 
                                  y(static_cast<T>(other.y)), 
                                  z(static_cast<T>(other.z)), 
                                  w(static_cast<T>(other.w)) {}
    
    // Conversion from Vec3
    template<typename U>
    Vec4(const Vec3<U>& other, T w = T(1)) : x(static_cast<T>(other.x)), 
                                              y(static_cast<T>(other.y)), 
                                              z(static_cast<T>(other.z)), 
                                              w(w) {}
    
    // Access operators
    T& operator[](size_t i) {
        if (i == 0) return x;
        if (i == 1) return y;
        if (i == 2) return z;
        return w;
    }
    
    const T& operator[](size_t i) const {
        if (i == 0) return x;
        if (i == 1) return y;
        if (i == 2) return z;
        return w;
    }
    
    // Assignment operators
    Vec4& operator+=(const Vec4& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    
    Vec4& operator-=(const Vec4& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    
    Vec4& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        w *= scalar;
        return *this;
    }
    
    Vec4& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }
    
    // Unary operators
    Vec4 operator+() const { return *this; }
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }
    
    // Binary operators
    Vec4 operator+(const Vec4& other) const { 
        return Vec4(x + other.x, y + other.y, z + other.z, w + other.w); 
    }
    Vec4 operator-(const Vec4& other) const { 
        return Vec4(x - other.x, y - other.y, z - other.z, w - other.w); 
    }
    Vec4 operator*(T scalar) const { 
        return Vec4(x * scalar, y * scalar, z * scalar, w * scalar); 
    }
    Vec4 operator/(T scalar) const { 
        return Vec4(x / scalar, y / scalar, z / scalar, w / scalar); 
    }
    
    friend Vec4 operator*(T scalar, const Vec4& v) { return v * scalar; }
    
    // Comparison
    bool operator==(const Vec4& other) const { 
        return x == other.x && y == other.y && z == other.z && w == other.w; 
    }
    bool operator!=(const Vec4& other) const { return !(*this == other); }
    
    // Vector operations
    T length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
    T lengthSquared() const { return x * x + y * y + z * z + w * w; }
    
    Vec4 normalized() const {
        T len = length();
        return (len > T(0)) ? (*this / len) : Vec4();
    }
    
    void normalize() {
        T len = length();
        if (len > T(0)) {
            *this /= len;
        }
    }
    
    T dot(const Vec4& other) const { 
        return x * other.x + y * other.y + z * other.z + w * other.w; 
    }
    
    /**
     * @brief Get the xyzw components as Vec3
     */
    Vec3<T> xyz() const { return Vec3<T>(x, y, z); }
    
    /**
     * @brief Get the xyz components as Vec3 (ignoring w)
     */
    Vec3<T> toVec3() const { return Vec3<T>(x, y, z); }
    
    /**
     * @brief Lerp between two vectors
     */
    static Vec4 lerp(const Vec4& a, const Vec4& b, T t) {
        return a + (b - a) * t;
    }
    
    std::string toString() const {
        return "(" + std::to_string(x) + ", " + 
               std::to_string(y) + ", " + 
               std::to_string(z) + ", " + 
               std::to_string(w) + ")";
    }
};

using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;
using Vec4i = Vec4<int32_t>;
using Vec4u = Vec4<uint32_t>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec4<T>& v) {
    os << v.toString();
    return os;
}

template<typename T>
const Vec4<T> Vec4<T>::ZERO{0, 0, 0, 0};

template<typename T>
const Vec4<T> Vec4<T>::ONE{1, 1, 1, 1};

} // namespace vgui
