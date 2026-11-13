// Quaternions in GLM's constructor order (w, x, y, z): products, rotation of vectors, normalize, angleAxis, slerp.
#pragma once
#include "scalar.hpp"
#include "vec.hpp"
#include "glsl.hpp"

namespace evgx { namespace math {

struct quat {
    float x, y, z, w;
    constexpr quat() : x(0), y(0), z(0), w(1) {}
    constexpr quat(float w_, float x_, float y_, float z_) : x(x_), y(y_), z(z_), w(w_) {}
};

// Hamilton product: applying (a * b) to a vector is the same as applying b first, then a
inline quat operator*(const quat& a, const quat& b) {
    return quat(a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
                a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
                a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x);
}

// rotates a vector by the quaternion
inline vec3<float> operator*(const quat& q, const vec3<float>& v) {
    vec3<float> u(q.x, q.y, q.z);
    vec3<float> t = cross(u, v) * 2.f;
    return v + t * q.w + cross(u, t);
}

inline float dot(const quat& a, const quat& b) { return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z; }

inline quat normalize(const quat& q) {
    float l = sqrt(dot(q, q));
    return l > 0 ? quat(q.w / l, q.x / l, q.y / l, q.z / l) : quat();
}

inline quat conjugate(const quat& q) { return quat(q.w, -q.x, -q.y, -q.z); }

// rotation of `angle` radians about `axis`
inline quat angleAxis(float angle, const vec3<float>& axis) {
    vec3<float> a = normalize(axis);
    float s = sin(angle * 0.5f);
    return quat(cos(angle * 0.5f), a.x * s, a.y * s, a.z * s);
}

// spherical interpolation; falls back to normalized lerp when the rotations are nearly equal
inline quat slerp(const quat& a, quat b, float t) {
    float d = dot(a, b);
    if (d < 0) { b = quat(-b.w, -b.x, -b.y, -b.z); d = -d; }
    if (d > 0.9995f)
        return normalize(quat(a.w + (b.w - a.w) * t, a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t));
    float s = sqrt(1.f - d * d);                  // sin(theta)
    float theta = atan2(s, d);
    float wa = sin((1.f - t) * theta) / s, wb = sin(t * theta) / s;
    return quat(a.w * wa + b.w * wb, a.x * wa + b.x * wb, a.y * wa + b.y * wb, a.z * wa + b.z * wb);
}

}}  // namespace evgx::math
