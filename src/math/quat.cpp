// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Quat  –  implementation                                ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/math/quat.hpp>
#include <cmath>
#include <cstdio>
#include <algorithm>

namespace vgui {

Quat Quat::operator*(const Quat& o) const {
    return {
        w*o.x + x*o.w + y*o.z - z*o.y,
        w*o.y - x*o.z + y*o.w + z*o.x,
        w*o.z + x*o.y - y*o.x + z*o.w,
        w*o.w - x*o.x - y*o.y - z*o.z
    };
}

Vec3 Quat::operator*(const Vec3& v) const {
    Vec3 qv{x, y, z};
    Vec3 uv  = Vec3::cross(qv, v);
    Vec3 uuv = Vec3::cross(qv, uv);
    return v + ((uv * w) + uuv) * 2.0f;
}

Quat Quat::normalized() const {
    float l = length();
    return l > 0 ? Quat{x/l, y/l, z/l, w/l} : identity();
}

Mat4 Quat::to_mat4() const {
    Quat q = normalized();
    Mat4 m = Mat4::identity();
    m.m[0][0] = 1 - 2*(q.y*q.y + q.z*q.z);
    m.m[0][1] =     2*(q.x*q.y + q.z*q.w);
    m.m[0][2] =     2*(q.x*q.z - q.y*q.w);
    m.m[1][0] =     2*(q.x*q.y - q.z*q.w);
    m.m[1][1] = 1 - 2*(q.x*q.x + q.z*q.z);
    m.m[1][2] =     2*(q.y*q.z + q.x*q.w);
    m.m[2][0] =     2*(q.x*q.z + q.y*q.w);
    m.m[2][1] =     2*(q.y*q.z - q.x*q.w);
    m.m[2][2] = 1 - 2*(q.x*q.x + q.y*q.y);
    return m;
}

Quat Quat::from_euler(float pitch, float yaw, float roll) {
    float cp = std::cos(pitch*.5f), sp = std::sin(pitch*.5f);
    float cy = std::cos(yaw  *.5f), sy = std::sin(yaw  *.5f);
    float cr = std::cos(roll *.5f), sr = std::sin(roll *.5f);
    return {
        cr*sp*cy + sr*cp*sy,
        cr*cp*sy - sr*sp*cy,
        sr*cp*cy - cr*sp*sy,
        cr*cp*cy + sr*sp*sy
    };
}

Quat Quat::slerp(const Quat& a, const Quat& b, float t) {
    float dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    Quat  bb  = dot < 0 ? Quat{-b.x,-b.y,-b.z,-b.w} : b;
    dot = std::abs(dot);
    if (dot > 0.9995f) {
        // Linear interpolation for nearly-identical quats
        return Quat{a.x+(bb.x-a.x)*t, a.y+(bb.y-a.y)*t, a.z+(bb.z-a.z)*t, a.w+(bb.w-a.w)*t}.normalized();
    }
    float theta0 = std::acos(dot);
    float theta  = theta0 * t;
    float s0 = std::cos(theta) - dot * std::sin(theta) / std::sin(theta0);
    float s1 = std::sin(theta) / std::sin(theta0);
    return { s0*a.x+s1*bb.x, s0*a.y+s1*bb.y, s0*a.z+s1*bb.z, s0*a.w+s1*bb.w };
}

Quat Quat::lerp(const Quat& a, const Quat& b, float t) {
    return Quat{a.x+(b.x-a.x)*t, a.y+(b.y-a.y)*t, a.z+(b.z-a.z)*t, a.w+(b.w-a.w)*t}.normalized();
}

std::string Quat::to_string() const {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Quat(%.4f, %.4f, %.4f, %.4f)", x, y, z, w);
    return buf;
}

} // namespace vgui
