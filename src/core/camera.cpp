// Camera controllers: Camera::fly (free flight) and Orbit (turntable around a model).
#include "state.hpp"

namespace evgx {
using namespace math;
using namespace st;

void Camera::fly(const Window& w, float speed) {
    if (w.button(1)) {                              // right-drag to look around
        yaw += w.dx * 0.0035f;
        pitch -= w.dy * 0.0035f;
        pitch = clamp(pitch, -1.55f, 1.55f);
    }
    float s = speed * (w.key_down(key::Shift) ? 3.f : 1.f) * w.dt;
    vec3<float> f = forward(), r = right(), up(0.f, 1.f, 0.f);
    if (w.key_down('W')) pos += f * s;
    if (w.key_down('S')) pos -= f * s;
    if (w.key_down('D')) pos += r * s;
    if (w.key_down('A')) pos -= r * s;
    if (w.key_down('E') || w.key_down(key::Space)) pos += up * s;
    if (w.key_down('Q') || w.key_down(key::Ctrl)) pos -= up * s;
}

void Orbit::update(const Window& w) {
    if (w.button(0)) {                              // left-drag to orbit
        yaw += w.dx * 0.01f;
        pitch = clamp(pitch + w.dy * 0.01f, -1.5f, 1.5f);
    }
    int n = (int)abs(w.wheel);
    if (n > 20) n = 20;
    for (int i = 0; i < n; i++) dist *= (w.wheel > 0 ? 0.9f : 1.f / 0.9f);
    dist = clamp(dist, 0.01f, 100000.f);
}

void Orbit::frame(const Model* m) {
    if (!m) return;
    target = (m->bmin + m->bmax) * 0.5f;
    vec3<float> ext = m->bmax - m->bmin;
    float r = length(ext) * 0.5f;
    dist = r < 1e-4f ? 3.f : r * 2.2f;
}

Camera Orbit::camera() const {
    Camera c;
    float cp = cos(pitch);
    vec3<float> off(sin(yaw) * cp, sin(pitch), cos(yaw) * cp);
    c.pos = target + off * dist;
    c.yaw = -yaw; c.pitch = -pitch;
    c.znear = dist * 0.01f; c.zfar = dist * 100.f + 10.f;
    return c;
}

}  // namespace evgx
