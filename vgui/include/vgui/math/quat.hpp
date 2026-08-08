#pragma once

#include <cmath>
#include <ostream>
#include "vgui/math/vec3.hpp"

namespace vgui {

/**
 * @brief Quaternion for 3D rotation representation
 * 
 * Uses scalar-first layout (w, x, y, z).
 * Provides smooth interpolation and avoids gimbal lock.
 */
template<typename T>
struct Quat {
    T w, x, y, z;
    
    // Constructors
    Quat() : w(1), x(0), y(0), z(0) {}
    Quat(T w, T x, T y, T z) : w(w), x(x), y(y), z(z) {}
    
    // Create from axis-angle
    Quat(const Vec3<T>& axis, T angle) {
        Vec3<T> a = axis.normalized();
        T halfAngle = angle * T(0.5);
        w = std::cos(halfAngle);
        T sinHalf = std::sin(halfAngle);
        x = a.x * sinHalf;
        y = a.y * sinHalf;
        z = a.z * sinHalf;
    }
    
    // Create from Euler angles (YXZ order, radians)
    Quat(T yaw, T pitch, T roll) {
        T cy = std::cos(yaw * T(0.5));
        T sy = std::sin(yaw * T(0.5));
        T cp = std::cos(pitch * T(0.5));
        T sp = std::sin(pitch * T(0.5));
        T cr = std::cos(roll * T(0.5));
        T sr = std::sin(roll * T(0.5));
        
        w = cr * cp * cy + sr * sp * sy;
        x = sr * cp * cy - cr * sp * sy;
        y = cr * sp * cy + sr * cp * sy;
        z = cr * cp * sy - sr * sp * cy;
    }
    
    // Create from rotation matrix
    explicit Quat(const class Mat4<T>& m) {
        // Extract from 3x3 rotation part
        T trace = m(0, 0) + m(1, 1) + m(2, 2);
        
        if (trace > T(0)) {
            T s = std::sqrt(trace + T(1)) * T(0.5);
            w = T(0.25) / s;
            x = (m(2, 1) - m(1, 2)) * s;
            y = (m(0, 2) - m(2, 0)) * s;
            z = (m(1, 0) - m(0, 1)) * s;
        } else if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2)) {
            T s = std::sqrt(T(1) + m(0, 0) - m(1, 1) - m(2, 2)) * T(0.5);
            w = (m(2, 1) - m(1, 2)) * T(0.25) / s;
            x = s;
            y = (m(0, 1) + m(1, 0)) * T(0.5) / s;
            z = (m(0, 2) + m(2, 0)) * T(0.5) / s;
        } else if (m(1, 1) > m(2, 2)) {
            T s = std::sqrt(T(1) + m(1, 1) - m(0, 0) - m(2, 2)) * T(0.5);
            w = (m(0, 2) - m(2, 0)) * T(0.25) / s;
            x = (m(0, 1) + m(1, 0)) * T(0.5) / s;
            y = s;
            z = (m(1, 2) + m(2, 1)) * T(0.5) / s;
        } else {
            T s = std::sqrt(T(1) + m(2, 2) - m(0, 0) - m(1, 1)) * T(0.5);
            w = (m(1, 0) - m(0, 1)) * T(0.25) / s;
            x = (m(0, 2) + m(2, 0)) * T(0.5) / s;
            y = (m(1, 2) + m(2, 1)) * T(0.5) / s;
            z = s;
        }
    }
    
    // Assignment operators
    Quat& operator+=(const Quat& other) {
        w += other.w; x += other.x; y += other.y; z += other.z;
        return *this;
    }
    
    Quat& operator-=(const Quat& other) {
        w -= other.w; x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    
    Quat& operator*=(T scalar) {
        w *= scalar; x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }
    
    Quat& operator/=(T scalar) {
        T inv = T(1) / scalar;
        w *= inv; x *= inv; y *= inv; z *= inv;
        return *this;
    }
    
    // Unary operators
    Quat operator+() const { return *this; }
    Quat operator-() const { return Quat(-w, -x, -y, -z); }
    
    // Binary operators
    Quat operator+(const Quat& other) const {
        return Quat(w + other.w, x + other.x, y + other.y, z + other.z);
    }
    
    Quat operator-(const Quat& other) const {
        return Quat(w - other.w, x - other.x, y - other.y, z - other.z);
    }
    
    Quat operator*(T scalar) const {
        return Quat(w * scalar, x * scalar, y * scalar, z * scalar);
    }
    
    Quat operator/(T scalar) const {
        T inv = T(1) / scalar;
        return Quat(w * inv, x * inv, y * inv, z * inv);
    }
    
    /**
     * @brief Quaternion multiplication (combined rotation)
     */
    Quat operator*(const Quat& other) const {
        return Quat(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }
    
    // Comparison
    bool operator==(const Quat& other) const {
        return w == other.w && x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const Quat& other) const {
        return !(*this == other);
    }
    
    // Quaternion operations
    T length() const {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }
    
    T lengthSquared() const {
        return w * w + x * x + y * y + z * z;
    }
    
    Quat normalized() const {
        T len = length();
        return (len > T(0)) ? (*this / len) : identity();
    }
    
    void normalize() {
        T len = length();
        if (len > T(0)) {
            *this /= len;
        }
    }
    
    /**
     * @brief Conjugate of the quaternion
     */
    Quat conjugate() const {
        return Quat(w, -x, -y, -z);
    }
    
    /**
     * @brief Inverse of the quaternion
     */
    Quat inverse() const {
        T lenSq = lengthSquared();
        if (lenSq > T(0)) {
            T inv = T(1) / lenSq;
            return Quat(w * inv, -x * inv, -y * inv, -z * inv);
        }
        return identity();
    }
    
    /**
     * @brief Rotate a vector by this quaternion
     */
    Vec3<T> rotate(const Vec3<T>& v) const {
        Quat q(v.x, v.y, v.z, T(0));
        Quat result = *this * q * conjugate();
        return Vec3<T>(result.x, result.y, result.z);
    }
    
    /**
     * @brief Convert to rotation matrix
     */
    Mat4<T> toMatrix() const;
    
    /**
     * @brief Get the rotation axis
     * @return Normalized axis of rotation, or zero if identity
     */
    Vec3<T> axis() const {
        T sin2 = x * x + y * y + z * z;
        if (sin2 > T(0)) {
            T sinHalf = std::sqrt(sin2);
            return Vec3<T>(x, y, z) / sinHalf;
        }
        return Vec3<T>::UNIT_X; // Identity rotation has no axis
    }
    
    /**
     * @brief Get the rotation angle
     * @return Angle of rotation in radians
     */
    T angle() const {
        return T(2) * std::acos(std::abs(w));
    }
    
    /**
     * @brief Get Euler angles (YXZ order, radians)
     */
    Vec3<T> toEuler() const {
        T sqw = w * w;
        T sqx = x * x;
        T sqy = y * y;
        T sqz = z * z;
        
        // Roll (x-axis rotation)
        T roll = std::atan2(T(2) * (w * x + y * z), T(1) - T(2) * (sqx + sqy));
        
        // Pitch (y-axis rotation)
        T pitch = std::asin(T(2) * (w * y - z * x));
        
        // Yaw (z-axis rotation)
        T yaw = std::atan2(T(2) * (w * z + x * y), T(1) - T(2) * (sqy + sqz));
        
        return Vec3<T>(yaw, pitch, roll);
    }
    
    // Static factory methods
    static Quat identity() {
        return Quat(T(1), T(0), T(0), T(0));
    }
    
    static Quat fromAxisAngle(const Vec3<T>& axis, T angle) {
        return Quat(axis, angle);
    }
    
    static Quat fromEuler(T yaw, T pitch, T roll) {
        return Quat(yaw, pitch, roll);
    }
    
    static Quat fromEuler(const Vec3<T>& euler) {
        return Quat(euler.x, euler.y, euler.z);
    }
    
    /**
     * @brief Spherical linear interpolation
     */
    static Quat slerp(const Quat& a, const Quat& b, T t) {
        T cosTheta = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
        
        // Handle sign
        Quat b2 = (cosTheta < T(0)) ? Quat(-b.w, -b.x, -b.y, -b.z) : b;
        cosTheta = std::abs(cosTheta);
        
        if (cosTheta > T(1) - T(1e-6)) {
            // Close enough to use linear interpolation
            return (a + (b2 - a) * t).normalized();
        }
        
        T theta = std::acos(cosTheta);
        T sinTheta = std::sin(theta);
        T invSinTheta = T(1) / sinTheta;
        
        T s1 = std::sin((T(1) - t) * theta) * invSinTheta;
        T s2 = std::sin(t * theta) * invSinTheta;
        
        return Quat(
            a.w * s1 + b2.w * s2,
            a.x * s1 + b2.x * s2,
            a.y * s1 + b2.y * s2,
            a.z * s1 + b2.z * s2
        );
    }
    
    /**
     * @brief Get the difference quaternion (a^-1 * b)
     */
    static Quat difference(const Quat& a, const Quat& b) {
        return a.inverse() * b;
    }
    
    std::string toString() const {
        return "Quat(" + std::to_string(w) + ", " + 
               std::to_string(x) + ", " + 
               std::to_string(y) + ", " + 
               std::to_string(z) + ")";
    }
};

using Quatf = Quat<float>;
using Quatd = Quat<double>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const Quat<T>& q) {
    os << q.toString();
    return os;
}

// Scalar multiplication (scalar * quat)
template<typename T>
Quat<T> operator*(T scalar, const Quat<T>& q) {
    return q * scalar;
}

} // namespace vgui
