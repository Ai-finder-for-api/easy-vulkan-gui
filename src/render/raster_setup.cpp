// Triangle setup: screen-space coordinates, orientation, bounding box and edge functions for one triangle.
#include "raster_internal.hpp"

namespace evgx { namespace raster {

namespace {
inline float f_min3(float a, float b, float c) { return math::min(a, math::min(b, c)); }
inline float f_max3(float a, float b, float c) { return math::max(a, math::max(b, c)); }
}

SV lerp_sv(const SV& a, const SV& b, float t) {
    SV r;
    r.clip = a.clip + (b.clip - a.clip) * t;
    r.n = a.n + (b.n - a.n) * t;
    r.wp = a.wp + (b.wp - a.wp) * t;
    r.col = a.col + (b.col - a.col) * t;
    return r;
}

bool tri_setup(const Window* w, const SV* v, const Material& mat, TriSetup& t) {
    using namespace math;
    const float W = (float)w->fbw, H = (float)w->fbh;
    for (int i = 0; i < 3; i++) {
        t.iw[i] = 1.f / v[i].clip.w;
        t.sx[i] = (v[i].clip.x * t.iw[i] * 0.5f + 0.5f) * W;
        t.sy[i] = (0.5f - v[i].clip.y * t.iw[i] * 0.5f) * H;
        t.sz[i] = v[i].clip.z * t.iw[i] * 0.5f + 0.5f;
    }
    for (int i = 0; i < 3; i++)                                   // rejects NaN/inf/absurd coordinates before any float->int cast
        if (!(t.sx[i] > -1e9f && t.sx[i] < 1e9f && t.sy[i] > -1e9f && t.sy[i] < 1e9f && t.sz[i] == t.sz[i])) return false;

    float S = (t.sx[1] - t.sx[0]) * (t.sy[2] - t.sy[0]) - (t.sx[2] - t.sx[0]) * (t.sy[1] - t.sy[0]);
    if (S == 0.f || !(S == S)) return false;                      // degenerate
    t.back = S > 0.f;                                             // CCW front faces have S < 0 in y-down screen space
    if (t.back && !mat.double_sided) return false;

    float fx0 = floor(f_min3(t.sx[0], t.sx[1], t.sx[2])), fx1 = floor(f_max3(t.sx[0], t.sx[1], t.sx[2]));
    float fy0 = floor(f_min3(t.sy[0], t.sy[1], t.sy[2])), fy1 = floor(f_max3(t.sy[0], t.sy[1], t.sy[2]));
    if (fx0 < 0.f) fx0 = 0.f;                                     // clamp in float, then cast: always in range
    if (fy0 < 0.f) fy0 = 0.f;
    if (fx1 > W - 1.f) fx1 = W - 1.f;
    if (fy1 > H - 1.f) fy1 = H - 1.f;
    if (fx0 > fx1 || fy0 > fy1) return false;
    t.x0 = (int)fx0; t.x1 = (int)fx1; t.y0 = (int)fy0; t.y1 = (int)fy1;

    // edge function for the edge opposite vertex e: F = A*px + B*py + C, normalized by 1/S to give barycentrics
    float invS = 1.f / S;
    for (int e = 0; e < 3; e++) {
        int p = (e + 1) % 3, q = (e + 2) % 3;                     // edge runs p -> q
        float dx = t.sx[q] - t.sx[p], dy = t.sy[q] - t.sy[p];
        t.A[e] = -dy * invS;
        t.B[e] = dx * invS;
        t.C[e] = (-dx * t.sy[p] + dy * t.sx[p]) * invS;
    }
    return true;
}

}}  // namespace evgx::raster
