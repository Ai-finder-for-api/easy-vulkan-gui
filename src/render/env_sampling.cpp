// Environment sampling: direction <-> equirectangular coordinates, texel fetch, bilinear filtering, and lighting_sample.
#include "env_internal.hpp"

namespace evgx { namespace env {
using namespace math;

void dir_to_uv(const vec3<float>& d, float* u, float* v) {
    *u = atan2(d.z, d.x) * (0.5f / pi) + 0.5f;
    *v = acos(d.y) * (1.f / pi);
}

vec3<float> uv_to_dir(float u, float v) {
    float phi = (u - 0.5f) * 2.f * pi, theta = v * pi;
    float st = sin(theta);
    return vec3<float>(st * cos(phi), cos(theta), st * sin(phi));
}

vec3<float> fetch(const float* img, int w, int h, int x, int y) {
    x = x < 0 ? 0 : (x >= w ? w - 1 : x);
    y = y < 0 ? 0 : (y >= h ? h - 1 : y);
    const float* p = img + ((size_t)y * w + x) * 3;
    return vec3<float>(p[0], p[1], p[2]);
}

vec3<float> bilinear(const float* img, int w, int h, float u, float v) {
    float fx = u * w - 0.5f, fy = v * h - 0.5f;
    int x0 = (int)floor(fx), y0 = (int)floor(fy);
    float tx = fx - x0, ty = fy - y0;
    int xw = ((x0 % w) + w) % w;                       // wrap horizontally
    int xw1 = (xw + 1) % w;
    int y0c = y0 < 0 ? 0 : (y0 >= h ? h - 1 : y0);
    int y1c = y0 + 1 < 0 ? 0 : (y0 + 1 >= h ? h - 1 : y0 + 1);
    vec3<float> a = fetch(img, w, h, xw, y0c), b = fetch(img, w, h, xw1, y0c);
    vec3<float> c = fetch(img, w, h, xw, y1c), d = fetch(img, w, h, xw1, y1c);
    return mix_v(mix_v(a, b, tx), mix_v(c, d, tx), ty);
}

}}  // namespace evgx::env

namespace evgx {
using namespace math;

vec3<float> lighting_sample(const Lighting& l, const vec3<float>& d, float roughness) {
    if (!l.rgb) return vec3<float>(0.f, 0.f, 0.f);
    float u, v;
    env::dir_to_uv(d, &u, &v);
    // roughness picks a mip level, blending the two nearest levels
    float r = clamp(roughness, 0.f, 1.f) * 5.f;
    int m0 = (int)floor(r);
    if (m0 >= l.nmips - 1) m0 = l.nmips - 1;
    float t = r - m0;
    if (m0 >= l.nmips - 1 || t < 1e-4f) return env::bilinear(l.mip[m0], l.mw[m0], l.mh[m0], u, v);
    vec3<float> a = env::bilinear(l.mip[m0], l.mw[m0], l.mh[m0], u, v);
    vec3<float> b = env::bilinear(l.mip[m0 + 1], l.mw[m0 + 1], l.mh[m0 + 1], u, v);
    return mix_v(a, b, t);
}

}  // namespace evgx
