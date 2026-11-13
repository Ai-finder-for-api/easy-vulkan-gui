// Roughness mip chain for specular: 2x2 box halving with a second pass for the coarser levels.
#include "env_internal.hpp"
#include "mem.hpp"

namespace evgx { namespace env {
using namespace math;

float* halve(const float* src, int w, int h, int* ow, int* oh) {
    int nw = w > 1 ? w / 2 : 1, nh = h > 1 ? h / 2 : 1;
    float* dst = alloc_arr<float>((size_t)nw * nh * 3);
    if (!dst) return nullptr;
    for (int y = 0; y < nh; y++)
        for (int x = 0; x < nw; x++) {
            vec3<float> s(0.f, 0.f, 0.f);
            for (int dy = 0; dy < 2; dy++)
                for (int dx = 0; dx < 2; dx++)
                    s += fetch(src, w, h, (x * 2 + dx) % w, (y * 2 + dy) % h);
            s = s * 0.25f;
            float* o = dst + ((size_t)y * nw + x) * 3;
            o[0] = s.x; o[1] = s.y; o[2] = s.z;
        }
    *ow = nw;
    *oh = nh;
    return dst;
}

void build_mips(Lighting* out) {
    out->nmips = 1;
    out->mw[0] = out->w;
    out->mh[0] = out->h;
    out->mip[0] = out->rgb;
    for (int m = 1; m < 6; m++) {
        int nw, nh;
        float* p = halve(out->mip[m - 1], out->mw[m - 1], out->mh[m - 1], &nw, &nh);
        if (!p) break;
        if (m > 1) {                                    // second blur pass: a cheap stand-in for a wider kernel
            float* q = halve(p, nw, nh, &nw, &nh);
            mem_free(p);
            if (!q) break;
            p = q;
        }
        out->mip[m] = p;
        out->mw[m] = nw;
        out->mh[m] = nh;
        out->nmips = m + 1;
    }
}

}}  // namespace evgx::env
