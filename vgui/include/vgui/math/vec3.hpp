#pragma once

#include <cmath>
#include <ostream>
#include "vgui/math/vec2.hpp"

namespace vgui {

/**
 * @brief 3D vector with basic operations
 */
template<typename T>
struct Vec3 {
    T x, y, z;
    
    // Constructors
    Vec3() : x(0), y(0), z(0) {}
    Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
    
    // Conversion from Vec2
    Vec3(T x, T y) : x(x), y(y), z(0) {}
    
    // Conversion constructor
    template<typename U>
    Vec3(const Vec3<U>& other) : x(static_cast<T>(other.x)), 
                                  y(static_cast<T>(other.y)), 
                                  z(static_cast<T>(other.z)) {}
    
    // Conversion from Vec2
    template<typename U>
    Vec3(const Vec2<U>& other, T z = T(0)) : x(static_cast<T>(other.x)), 
                                              y(static_cast<T>(other.y)), 
                                              z(z) {}
    
    // Access operators
    T& operator[](size_t i) {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    
    const T& operator[](size_t i) const {
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    
    // Assignment operators
    Vec3& operator+=(const Vec3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    Vec3& operator-=(const Vec3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    Vec3& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    Vec3& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    
    // Unary operators
    Vec3 operator+() const { return *this; }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    
    // Binary operators
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(T scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(T scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    
    friend Vec3 operator*(T scalar, const Vec3& v) { return v * scalar; }
    
    // Comparison
    bool operator==(const Vec3& other) const { 
        return x == other.x && y == other.y && z == other.z; 
    }
    bool operator!=(const Vec3& other) const { return !(*this == other); }
    
    // Vector operations
    T length() const { return std::sqrt(x * x + y * y + z * z); }
    T lengthSquared() const { return x * x + y * y + z * z; }
    
    Vec3 normalized() const {
        T len = length();
        return (len > T(0)) ? (*this / len) : Vec3();
    }
    
    void normalize() {
        T len = length();
        if (len > T(0)) {
            *this /= len;
        }
    }
    
    T dot(const Vec3& other) const { 
        return x * other.x + y * other.y + z * other.z; 
    }
    
    /**
     * @brief 3D cross product
     */
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    /**
     * @brief Reflect vector around a normal
     */
    Vec3 reflect(const Vec3& normal) const {
        return *this - normal * (T(2) * dot(normal));
    }
    
    /**
     * @brief Refract vector through a surface
     * @param normal Surface normal (should be normalized)
     * @param ior Index of refraction
     * @return Refracted vector, or zero vector if total internal reflection
     */
    Vec3 refract(const Vec3& normal, T ior) const {
        T cosI = -dot(normal);
        T sinI2 = (T(1) - cosI * cosI) * ior * ior;
        
        if (sinI2 > T(1)) {
            return Vec3(); // Total internal reflection
        }
        
        T cosT = std::sqrt(T(1) - sinI2);
        return (*this * ior + normal * (ior * cosI - cosT));
    }
    
    /**
     * @brief Project onto another vector
     */
    Vec3 project(const Vec3& onto) const {
        T dotProduct = dot(onto);
        T ontoLenSq = onto.lengthSquared();
        return onto * (dotProduct / ontoLenSq);
    }
    
    /**
     * @brief Reject from another vector
     */
    Vec3 reject(const Vec3& from) const {
        return *this - project(from);
    }
    
    /**
     * @brief Lerp between two vectors
     */
    static Vec3 lerp(const Vec3& a, const Vec3& b, T t) {
        return a + (b - a) * t;
    }
    
    /**
     * @brief Distance between two points
     */
    static T distance(const Vec3& a, const Vec3& b) {
        return (b - a).length();
    }
    
    static T distanceSquared(const Vec3& a, const Vec3& b) {
        return (b - a).lengthSquared();
    }
    
    // Common vector constants
    static const Vec3 ZERO;
    static const Vec3 ONE;
    static const Vec3 UNIT_X;
    static const Vec3 UNIT_Y;
    static const Vec3 UNIT_Z;
    static const Vec3 NEG_UNIT_X;
    static const Vec3 NEG_UNIT_Y;
    static const Vec3 NEG_UNIT_Z;
    
    std::string toString() const {
        return "(" + std::to_string(x) + ", " + 
               std::to_string(y) + ", " + std::to_string(z) + ")";
    }
};

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<int32_t>;
using Vec3u = Vec3<uint32_t>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec3<T>& v) {
    os << v.toString();
    return os;
}

template<typename T>
const Vec3<T> Vec3<T>::ZERO{0, 0, 0};

template<typename T>
const Vec3<T> Vec3<T>::ONE{1, 1, 1};

template<typename T>
const Vec3<T> Vec3<T>::UNIT_X{1, 0, 0};

template<typename T>
const Vec3<T> Vec3<T>::UNIT_Y{0, 1, 0};

template<typename T>
const Vec3<T> Vec3<T>::UNIT_Z{0, 0, 1};

template<typename T>
const Vec3<T> Vec3<T>::NEG_UNIT_X{-1, 0, 0};

template<typename T>
const Vec3<T> Vec3<T>::NEG_UNIT_Y{0, -1, 0};

template<typename T>
const Vec3<T> Vec3<T>::NEG_UNIT_Z{0, 0, -1};

} // namespace vgui
