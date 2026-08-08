#pragma once

#include <cmath>
#include <ostream>

namespace vgui {

/**
 * @brief 2D vector with basic operations
 * @tparam T Component type (float, double, int, etc.)
 */
template<typename T>
struct Vec2 {
    T x, y;
    
    // Constructors
    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}
    
    // Conversion constructor
    template<typename U>
    Vec2(const Vec2<U>& other) : x(static_cast<T>(other.x)), 
                                  y(static_cast<T>(other.y)) {}
    
    // Access operators
    T& operator[](size_t i) {
        return (i == 0) ? x : y;
    }
    
    const T& operator[](size_t i) const {
        return (i == 0) ? x : y;
    }
    
    // Assignment operators
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Vec2& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    
    Vec2& operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }
    
    // Unary operators
    Vec2 operator+() const { return *this; }
    Vec2 operator-() const { return Vec2(-x, -y); }
    
    // Binary operators
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(T scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(T scalar) const { return Vec2(x / scalar, y / scalar); }
    
    friend Vec2 operator*(T scalar, const Vec2& v) { return v * scalar; }
    friend Vec2 operator/(T scalar, const Vec2& v) { return Vec2(scalar / v.x, scalar / v.y); }
    
    // Comparison
    bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return !(*this == other); }
    
    // Vector operations
    T length() const { return std::sqrt(x * x + y * y); }
    T lengthSquared() const { return x * x + y * y; }
    
    Vec2 normalized() const {
        T len = length();
        return (len > T(0)) ? (*this / len) : Vec2();
    }
    
    void normalize() {
        T len = length();
        if (len > T(0)) {
            *this /= len;
        }
    }
    
    T dot(const Vec2& other) const { return x * other.x + y * other.y; }
    
    /**
     * @brief 2D cross product (scalar)
     */
    T cross(const Vec2& other) const { return x * other.y - y * other.x; }
    
    /**
     * @brief Perpendicular vector (rotate 90 degrees CCW)
     */
    Vec2 perpendicular() const { return Vec2(-y, x); }
    
    /**
     * @brief Lerp between two vectors
     */
    static Vec2 lerp(const Vec2& a, const Vec2& b, T t) {
        return a + (b - a) * t;
    }
    
    /**
     * @brief Distance between two points
     */
    static T distance(const Vec2& a, const Vec2& b) {
        return (b - a).length();
    }
    
    /**
     * @brief Squared distance (faster, no sqrt)
     */
    static T distanceSquared(const Vec2& a, const Vec2& b) {
        return (b - a).lengthSquared();
    }
    
    // Common vector constants
    static const Vec2 ZERO;
    static const Vec2 ONE;
    static const Vec2 UNIT_X;
    static const Vec2 UNIT_Y;
    
    // String representation
    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

// Specializations for common types
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int32_t>;
using Vec2u = Vec2<uint32_t>;

// Stream output
template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec2<T>& v) {
    os << v.toString();
    return os;
}

// Define static constants
template<typename T>
const Vec2<T> Vec2<T>::ZERO{0, 0};

template<typename T>
const Vec2<T> Vec2<T>::ONE{1, 1};

template<typename T>
const Vec2<T> Vec2<T>::UNIT_X{1, 0};

template<typename T>
const Vec2<T> Vec2<T>::UNIT_Y{0, 1};

} // namespace vgui
