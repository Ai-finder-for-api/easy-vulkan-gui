// Scanline fill of one set-up triangle: barycentric stepping, depth test, perspective-correct attributes, shading.
#include "raster_internal.hpp"

namespace evgx { namespace raster {
using namespace math;

void tri_fill(Window* w, const SV* v, const TriSetup& t, const Material& mat, const FrameState& fs) {
    const float eps = -1e-5f;                                  // tolerance on shared edges
    vec3<float> N0 = v[0].n, N1 = v[1].n, N2 = v[2].n;
    for (int y = t.y0; y <= t.y1; y++) {
        float py = y + 0.5f, px = t.x0 + 0.5f;
        float l0 = t.A[0] * px + t.B[0] * py + t.C[0];
        float l1 = t.A[1] * px + t.B[1] * py + t.C[1];
        float l2 = t.A[2] * px + t.B[2] * py + t.C[2];
        for (int x = t.x0; x <= t.x1; x++, l0 += t.A[0], l1 += t.A[1], l2 += t.A[2]) {
            if (l0 < eps || l1 < eps || l2 < eps) continue;       // outside the triangle
            float z = l0 * t.sz[0] + l1 * t.sz[1] + l2 * t.sz[2];
            if (z < 0.f || z > 1.f) continue;
            size_t idx = (size_t)y * w->fbw + x;
            if (z >= w->depth[idx]) continue;                     // hidden by something nearer
            w->depth[idx] = z;

            // perspective-correct barycentrics: weights divided by w, then renormalized
            float ip = l0 * t.iw[0] + l1 * t.iw[1] + l2 * t.iw[2];
            float k = 1.f / ip;
            float b0 = l0 * t.iw[0] * k, b1 = l1 * t.iw[1] * k, b2 = l2 * t.iw[2] * k;
            vec3<float> n = N0 * b0 + N1 * b1 + N2 * b2;
            vec3<float> wp = v[0].wp * b0 + v[1].wp * b1 + v[2].wp * b2;
            vec3<float> col = v[0].col * b0 + v[1].col * b1 + v[2].col * b2;
            float nl = length(n);
            vec3<float> N = nl > 1e-8f ? n * (1.f / nl) : vec3<float>(0.f, 1.f, 0.f);
            if (t.back) N = -N;
            vec3<float> V = normalize(fs.cam_pos - wp);
            vec3<float> out = shade_surface(mat, fs, N, V, col);
            w->color[idx] = pack_argb(out);
        }
    }
}

}}  // namespace evgx::raster
