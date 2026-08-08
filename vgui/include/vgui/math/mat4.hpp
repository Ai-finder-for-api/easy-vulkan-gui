#pragma once

#include <cmath>
#include <ostream>
#include <array>
#include "vgui/math/vec3.hpp"
#include "vgui/math/vec4.hpp"

namespace vgui {

/**
 * @brief 4x4 matrix with common transformations
 * 
 * Uses column-major layout (compatible with Vulkan/GLSL).
 * Matrix multiplication takes row * column.
 */
template<typename T>
struct Mat4 {
    std::array<T, 16> m{}; // Column-major: m[col * 4 + row]
    
    // Index access
    T& operator()(size_t col, size_t row) {
        return m[col * 4 + row];
    }
    
    const T& operator()(size_t col, size_t row) const {
        return m[col * 4 + row];
    }
    
    // Component access
    T& xx() { return m[0]; }
    T& yx() { return m[4]; }
    T& zx() { return m[8]; }
    T& wx() { return m[12]; }
    
    T& xy() { return m[1]; }
    T& yy() { return m[5]; }
    T& zy() { return m[9]; }
    T& wy() { return m[13]; }
    
    T& xz() { return m[2]; }
    T& yz() { return m[6]; }
    T& zz() { return m[10]; }
    T& wz() { return m[14]; }
    
    T& xw() { return m[3]; }
    T& yw() { return m[7]; }
    T& zw() { return m[11]; }
    T& ww() { return m[15]; }
    
    // Constructors
    Mat4() {
        // Initialize to identity
        *this = identity();
    }
    
    // Construct from 16 values (column-major)
    Mat4(T m00, T m01, T m02, T m03,
         T m10, T m11, T m12, T m13,
         T m20, T m21, T m22, T m23,
         T m30, T m31, T m32, T m33) {
        m = {m00, m01, m02, m03,
              m10, m11, m12, m13,
              m20, m21, m22, m23,
              m30, m31, m32, m33};
    }
    
    // Assignment operators
    Mat4& operator+=(const Mat4& other) {
        for (size_t i = 0; i < 16; ++i) m[i] += other.m[i];
        return *this;
    }
    
    Mat4& operator-=(const Mat4& other) {
        for (size_t i = 0; i < 16; ++i) m[i] -= other.m[i];
        return *this;
    }
    
    Mat4& operator*=(T scalar) {
        for (size_t i = 0; i < 16; ++i) m[i] *= scalar;
        return *this;
    }
    
    Mat4& operator/=(T scalar) {
        T inv = T(1) / scalar;
        for (size_t i = 0; i < 16; ++i) m[i] *= inv;
        return *this;
    }
    
    // Unary operators
    Mat4 operator+() const { return *this; }
    Mat4 operator-() const {
        Mat4 result;
        for (size_t i = 0; i < 16; ++i) result.m[i] = -m[i];
        return result;
    }
    
    // Binary operators
    Mat4 operator+(const Mat4& other) const {
        Mat4 result = *this;
        result += other;
        return result;
    }
    
    Mat4 operator-(const Mat4& other) const {
        Mat4 result = *this;
        result -= other;
        return result;
    }
    
    Mat4 operator*(T scalar) const {
        Mat4 result = *this;
        result *= scalar;
        return result;
    }
    
    Mat4 operator/(T scalar) const {
        Mat4 result = *this;
        result /= scalar;
        return result;
    }
    
    /**
     * @brief Matrix multiplication (this * other)
     */
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (size_t col = 0; col < 4; ++col) {
            for (size_t row = 0; row < 4; ++row) {
                result(row, col) = 
                    m[0*4+row] * other(col, 0) +
                    m[1*4+row] * other(col, 1) +
                    m[2*4+row] * other(col, 2) +
                    m[3*4+row] * other(col, 3);
            }
        }
        return result;
    }
    
    // Vector transformation
    Vec4<T> operator*(const Vec4<T>& v) const {
        return Vec4<T>(
            m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w,
            m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w,
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
            m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
        );
    }
    
    Vec3<T> operator*(const Vec3<T>& v) const {
        return Vec3<T>(
            m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12],
            m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13],
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]
        );
    }
    
    // Comparison
    bool operator==(const Mat4& other) const {
        for (size_t i = 0; i < 16; ++i) {
            if (m[i] != other.m[i]) return false;
        }
        return true;
    }
    
    bool operator!=(const Mat4& other) const {
        return !(*this == other);
    }
    
    // Static factory methods
    static Mat4 identity() {
        Mat4 result;
        result.m = {1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1};
        return result;
    }
    
    static Mat4 zero() {
        Mat4 result;
        result.m.fill(0);
        return result;
    }
    
    /**
     * @brief Create translation matrix
     */
    static Mat4 translate(const Vec3<T>& translation) {
        Mat4 result = identity();
        result.m[12] = translation.x;
        result.m[13] = translation.y;
        result.m[14] = translation.z;
        return result;
    }
    
    /**
     * @brief Create scale matrix
     */
    static Mat4 scale(const Vec3<T>& scale) {
        Mat4 result;
        result.m = {scale.x, 0, 0, 0,
                    0, scale.y, 0, 0,
                    0, 0, scale.z, 0,
                    0, 0, 0, 1};
        return result;
    }
    
    /**
     * @brief Create scale matrix (uniform)
     */
    static Mat4 scale(T scale) {
        return scale(Vec3<T>(scale, scale, scale));
    }
    
    /**
     * @brief Create rotation matrix around X axis (radians)
     */
    static Mat4 rotateX(T angle) {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return Mat4(
            1, 0,  0, 0,
            0, c, -s, 0,
            0, s,  c, 0,
            0, 0,  0, 1
        );
    }
    
    /**
     * @brief Create rotation matrix around Y axis (radians)
     */
    static Mat4 rotateY(T angle) {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return Mat4(
            c, 0, s, 0,
            0, 1, 0, 0,
            -s, 0, c, 0,
            0, 0, 0, 1
        );
    }
    
    /**
     * @brief Create rotation matrix around Z axis (radians)
     */
    static Mat4 rotateZ(T angle) {
        T c = std::cos(angle);
        T s = std::sin(angle);
        return Mat4(
            c, -s, 0, 0,
            s,  c, 0, 0,
            0,  0, 1, 0,
            0,  0, 0, 1
        );
    }
    
    /**
     * @brief Create rotation matrix from quaternion
     */
    static Mat4 rotate(const class Quat<T>& q);
    
    /**
     * @brief Create rotation matrix from axis-angle
     */
    static Mat4 rotate(const Vec3<T>& axis, T angle) {
        Vec3<T> a = axis.normalized();
        T c = std::cos(angle);
        T s = std::sin(angle);
        T t = T(1) - c;
        
        return Mat4(
            a.x * a.x * t + c,      a.y * a.x * t + a.z * s,  a.z * a.x * t - a.y * s, 0,
            a.x * a.y * t - a.z * s, a.y * a.y * t + c,      a.z * a.y * t + a.x * s, 0,
            a.x * a.z * t + a.y * s, a.y * a.z * t - a.x * s, a.z * a.z * t + c,      0,
            0,                        0,                        0,                       1
        );
    }
    
    /**
     * @brief Create orthographic projection matrix
     */
    static Mat4 ortho(T left, T right, T bottom, T top, T near, T far) {
        T rl = T(1) / (right - left);
        T tb = T(1) / (top - bottom);
        T fn = T(1) / (far - near);
        
        return Mat4(
            T(2) * rl, 0,        0,         0,
            0,         T(2) * tb, 0,         0,
            0,         0,        T(2) * fn,  0,
            -(right + left) * rl, -(top + bottom) * tb, -(far + near) * fn, 1
        );
    }
    
    /**
     * @brief Create perspective projection matrix (Y-up, Vulkan-style)
     */
    static Mat4 perspective(T fovY, T aspect, T near, T far) {
        T tanHalfFov = std::tan(fovY * T(0.5));
        T zRange = far - near;
        
        Mat4 result;
        result.m[0] = T(1) / (aspect * tanHalfFov);
        result.m[5] = T(1) / tanHalfFov;
        result.m[10] = far / (near - far);  // Vulkan: [0,1] depth
        result.m[11] = T(1);
        result.m[14] = -(far * near) / zRange;
        result.m[15] = T(0);
        return result;
    }
    
    /**
     * @brief Create look-at view matrix
     */
    static Mat4 lookAt(const Vec3<T>& eye, 
                       const Vec3<T>& target, 
                       const Vec3<T>& up) {
        Vec3<T> forward = (target - eye).normalized();
        Vec3<T> right = forward.cross(up).normalized();
        Vec3<T> upActual = right.cross(forward);
        
        return Mat4(
            right.x, upActual.x, -forward.x, 0,
            right.y, upActual.y, -forward.y, 0,
            right.z, upActual.z, -forward.z, 0,
            -right.dot(eye), -upActual.dot(eye), forward.dot(eye), 1
        );
    }
    
    /**
     * @brief Transpose the matrix
     */
    Mat4 transpose() const {
        Mat4 result;
        for (size_t col = 0; col < 4; ++col) {
            for (size_t row = 0; row < 4; ++row) {
                result(row, col) = m[col * 4 + row];
            }
        }
        return result;
    }
    
    /**
     * @brief Invert the matrix
     */
    Mat4 inverse() const;
    
    /**
     * @brief Determinant of the matrix
     */
    T determinant() const;
    
    /**
     * @brief Get the 3x3 upper-left submatrix (rotation/scale part)
     */
    Mat3<T> upper3x3() const;
    
    std::string toString() const {
        std::string result = "Mat4(\n";
        for (size_t row = 0; row < 4; ++row) {
            result += "  ";
            for (size_t col = 0; col < 4; ++col) {
                result += std::to_string(m[col * 4 + row]);
                if (col < 3) result += ", ";
            }
            if (row < 3) result += "\n";
        }
        result += ")";
        return result;
    }
};

using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const Mat4<T>& m) {
    os << m.toString();
    return os;
}

// Forward declare Mat3 for upper3x3
template<typename T>
struct Mat3;

} // namespace vgui
