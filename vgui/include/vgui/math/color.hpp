#pragma once

#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include "vgui/math/vec4.hpp"

namespace vgui {

/**
 * @brief RGBA color representation
 * 
 * Color values are in linear space by default.
 * For sRGB conversion, use gamma functions.
 */
template<typename T>
struct Color {
    T r, g, b, a;
    
    // Constructors
    Color() : r(T(1)), g(T(1)), b(T(1)), a(T(1)) {}
    Color(T r, T g, T b, T a = T(1)) : r(r), g(g), b(b), a(a) {}
    
    // Grayscale constructor
    explicit Color(T gray, T alpha = T(1)) : r(gray), g(gray), b(gray), a(alpha) {}
    
    // Conversion from Vec4 (expects 0-1 range)
    explicit Color(const Vec4<T>& v) : r(v.x), g(v.y), b(v.z), a(v.w) {}
    
    // Conversion from 8-bit values
    static Color from8Bit(uint8_t r8, uint8_t g8, uint8_t b8, uint8_t a8 = 255) {
        return Color(
            T(r8) / T(255),
            T(g8) / T(255),
            T(b8) / T(255),
            T(a8) / T(255)
        );
    }
    
    // Conversion from hex string ("#RRGGBB" or "#AARRGGBB")
    static Color fromHex(const std::string& hex) {
        if (hex.empty() || hex[0] != '#') {
            return Color(T(1), T(1), T(1), T(1));
        }
        
        size_t len = hex.length();
        uint8_t r8 = 0, g8 = 0, b8 = 0, a8 = 255;
        
        if (len == 7) {
            // #RRGGBB
            r8 = static_cast<uint8_t>(std::stoul(hex.substr(1, 2), nullptr, 16));
            g8 = static_cast<uint8_t>(std::stoul(hex.substr(3, 2), nullptr, 16));
            b8 = static_cast<uint8_t>(std::stoul(hex.substr(5, 2), nullptr, 16));
        } else if (len == 9) {
            // #AARRGGBB
            a8 = static_cast<uint8_t>(std::stoul(hex.substr(1, 2), nullptr, 16));
            r8 = static_cast<uint8_t>(std::stoul(hex.substr(3, 2), nullptr, 16));
            g8 = static_cast<uint8_t>(std::stoul(hex.substr(5, 2), nullptr, 16));
            b8 = static_cast<uint8_t>(std::stoul(hex.substr(7, 2), nullptr, 16));
        }
        
        return from8Bit(r8, g8, b8, a8);
    }
    
    // Assignment operators
    Color& operator+=(const Color& other) {
        r += other.r; g += other.g; b += other.b; a += other.a;
        return *this;
    }
    
    Color& operator-=(const Color& other) {
        r -= other.r; g -= other.g; b -= other.b; a -= other.a;
        return *this;
    }
    
    Color& operator*=(T scalar) {
        r *= scalar; g *= scalar; b *= scalar; a *= scalar;
        return *this;
    }
    
    Color& operator/=(T scalar) {
        T inv = T(1) / scalar;
        r *= inv; g *= inv; b *= inv; a *= inv;
        return *this;
    }
    
    Color& operator*=(const Color& other) {
        r *= other.r; g *= other.g; b *= other.b; a *= other.a;
        return *this;
    }
    
    // Unary operators
    Color operator+() const { return *this; }
    
    // Binary operators
    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }
    
    Color operator-(const Color& other) const {
        return Color(r - other.r, g - other.g, b - other.b, a - other.a);
    }
    
    Color operator*(T scalar) const {
        return Color(r * scalar, g * scalar, b * scalar, a * scalar);
    }
    
    Color operator/(T scalar) const {
        T inv = T(1) / scalar;
        return Color(r * inv, g * inv, b * inv, a * inv);
    }
    
    Color operator*(const Color& other) const {
        return Color(r * other.r, g * other.g, b * other.b, a * other.a);
    }
    
    friend Color operator*(T scalar, const Color& c) { return c * scalar; }
    
    // Comparison
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
    
    bool operator!=(const Color& other) const {
        return !(*this == other);
    }
    
    // Clamp to valid range [0, 1]
    Color clamped() const {
        return Color(
            std::max(T(0), std::min(T(1), r)),
            std::max(T(0), std::min(T(1), g)),
            std::max(T(0), std::min(T(1), b)),
            std::max(T(0), std::min(T(1), a))
        );
    }
    
    // Convert to 8-bit values
    std::array<uint8_t, 4> to8Bit() const {
        return {
            static_cast<uint8_t>(std::round(r * 255)),
            static_cast<uint8_t>(std::round(g * 255)),
            static_cast<uint8_t>(std::round(b * 255)),
            static_cast<uint8_t>(std::round(a * 255))
        };
    }
    
    // Convert to hex string
    std::string toHex(bool includeAlpha = true) const {
        auto bytes = to8Bit();
        std::string result = "#";
        
        if (includeAlpha) {
            result += toHexByte(bytes[0]);
            result += toHexByte(bytes[1]);
            result += toHexByte(bytes[2]);
            result += toHexByte(bytes[3]);
        } else {
            result += toHexByte(bytes[0]);
            result += toHexByte(bytes[1]);
            result += toHexByte(bytes[2]);
        }
        
        return result;
    }
    
    // Gamma correction (sRGB)
    Color gammaCorrect(T gamma = T(2.2)) const {
        auto gammaCorrectChannel = [gamma](T c) -> T {
            return (c > T(0)) ? std::pow(c, T(1) / gamma) : T(0);
        };
        return Color(
            gammaCorrectChannel(r),
            gammaCorrectChannel(g),
            gammaCorrectChannel(b),
            a
        );
    }
    
    // Linearize (convert from sRGB to linear)
    Color linearize(T gamma = T(2.2)) const {
        auto linearizeChannel = [gamma](T c) -> T {
            return (c > T(0.04045)) ? 
                   std::pow((c + T(0.055)) / T(1.055), gamma) : 
                   c / T(12.92);
        };
        return Color(
            linearizeChannel(r),
            linearizeChannel(g),
            linearizeChannel(b),
            a
        );
    }
    
    // Convert to Vec4
    Vec4<T> toVec4() const {
        return Vec4<T>(r, g, b, a);
    }
    
    // Static color constants
    static const Color BLACK;
    static const Color WHITE;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color YELLOW;
    static const Color CYAN;
    static const Color MAGENTA;
    static const Color GRAY;
    static const Color TRANSPARENT;
    static const Color CLEAR;  // Completely transparent
    
    std::string toString() const {
        return "(" + std::to_string(r) + ", " + 
               std::to_string(g) + ", " + 
               std::to_string(b) + ", " + 
               std::to_string(a) + ")";
    }
    
private:
    static std::string toHexByte(uint8_t byte) {
        std::string result;
        uint8_t high = (byte >> 4) & 0xF;
        uint8_t low = byte & 0xF;
        
        result += (high < 10) ? ('0' + high) : ('A' + high - 10);
        result += (low < 10) ? ('0' + low) : ('A' + low - 10);
        
        return result;
    }
};

using Colorf = Color<float>;
using Colord = Color<double>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const Color<T>& c) {
    os << c.toString();
    return os;
}

// Define static constants
template<typename T>
const Color<T> Color<T>::BLACK = Color<T>(T(0), T(0), T(0), T(1));

template<typename T>
const Color<T> Color<T>::WHITE = Color<T>(T(1), T(1), T(1), T(1));

template<typename T>
const Color<T> Color<T>::RED = Color<T>(T(1), T(0), T(0), T(1));

template<typename T>
const Color<T> Color<T>::GREEN = Color<T>(T(0), T(1), T(0), T(1));

template<typename T>
const Color<T> Color<T>::BLUE = Color<T>(T(0), T(0), T(1), T(1));

template<typename T>
const Color<T> Color<T>::YELLOW = Color<T>(T(1), T(1), T(0), T(1));

template<typename T>
const Color<T> Color<T>::CYAN = Color<T>(T(0), T(1), T(1), T(1));

template<typename T>
const Color<T> Color<T>::MAGENTA = Color<T>(T(1), T(0), T(1), T(1));

template<typename T>
const Color<T> Color<T>::GRAY = Color<T>(T(0.5), T(0.5), T(0.5), T(1));

template<typename T>
const Color<T> Color<T>::TRANSPARENT = Color<T>(T(0), T(0), T(0), T(0));

template<typename T>
const Color<T> Color<T>::CLEAR = Color<T>(T(0), T(0), T(0), T(0));

// Predefined UI colors
namespace colors {
    constexpr Colorf primary{0.15f, 0.45f, 0.9f};
    constexpr Colorf secondary{0.2f, 0.2f, 0.25f};
    constexpr Colorf accent{0.95f, 0.5f, 0.1f};
    constexpr Colorf text{1.0f, 1.0f, 1.0f};
    constexpr Colorf textDark{0.2f, 0.2f, 0.2f};
    constexpr Colorf background{0.08f, 0.08f, 0.1f};
    constexpr Colorf hover{0.25f, 0.25f, 0.3f};
    constexpr Colorf active{0.35f, 0.35f, 0.4f};
    constexpr Colorf border{0.2f, 0.2f, 0.25f};
    constexpr Colorf error{0.9f, 0.2f, 0.2f};
    constexpr Colorf success{0.2f, 0.8f, 0.3f};
    constexpr Colorf warning{0.9f, 0.7f, 0.1f};
}

} // namespace vgui
