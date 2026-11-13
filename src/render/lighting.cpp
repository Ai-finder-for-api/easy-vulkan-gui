// Environment lighting entry points: build from linear RGB (copies and clamps the input), free. Sampling, SH and mips are in env_*.cpp.
#include <evg/lighting.hpp>
#include "env_internal.hpp"
#include "mem.hpp"

namespace evgx {

bool lighting_build(Lighting* out, const float* rgb, int w, int h) {
    if (!out || !rgb || w <= 0 || h <= 0) return false;
    *out = Lighting();
    out->w = w;
    out->h = h;
    out->rgb = alloc_arr<float>((size_t)w * h * 3);
    if (!out->rgb) return false;
    for (size_t i = 0; i < (size_t)w * h * 3; i++) out->rgb[i] = rgb[i] > 0.f ? rgb[i] : 0.f;   // radiance is non-negative

    env::build_irradiance(out, w, h);
    env::build_mips(out);
    return true;
}

void lighting_free(Lighting* l) {
    if (!l) return;
    for (int m = 1; m < l->nmips; m++) mem_free(l->mip[m]);     // mip 0 aliases rgb, so it is not freed twice
    mem_free(l->rgb);
    *l = Lighting();
}

}  // namespace evgx
