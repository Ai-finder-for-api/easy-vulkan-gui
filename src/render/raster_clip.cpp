// Clip-space clipping: cheap rejection against the side planes, then Sutherland-Hodgman against the near/far planes.
#include "raster_internal.hpp"

namespace evgx { namespace raster {
using namespace math;

namespace {
// runs setup + fill for one (already clipped) triangle
void draw_one(Window* w, const SV* v, const Material& mat, const FrameState& fs) {
    TriSetup t;
    if (tri_setup(w, v, mat, t)) tri_fill(w, v, t, mat, fs);
}
}

void clip_and_draw(Window* w, const SV* t, const Material& mat, const FrameState& fs) {
    float d[3];                                    // signed distance to the near plane (z + w >= 0 is inside)
    bool any_out = false;
    for (int i = 0; i < 3; i++) {
        d[i] = t[i].clip.z + t[i].clip.w;
        if (d[i] < 0.f) any_out = true;
    }
    // reject when all three vertices lie beyond the same left/right or top/bottom plane
    for (int axis = 0; axis < 2; axis++) {
        bool lo = true, hi = true;
        for (int i = 0; i < 3; i++) {
            float c = axis ? t[i].clip.y : t[i].clip.x;
            if (c > -t[i].clip.w) lo = false;
            if (c < t[i].clip.w) hi = false;
        }
        if (lo || hi) return;
    }
    if (!any_out) { draw_one(w, t, mat, fs); return; }

    // clip the polygon against the near plane: a triangle becomes up to a quad, fan-triangulated afterwards
    SV poly[4];
    int pn = 0;
    for (int i = 0; i < 3; i++) {
        int j = (i + 1) % 3;
        if (d[i] >= 0.f) poly[pn++] = t[i];
        if ((d[i] >= 0.f) != (d[j] >= 0.f)) poly[pn++] = lerp_sv(t[i], t[j], d[i] / (d[i] - d[j]));
    }
    for (int k = 1; k + 1 < pn; k++) {
        SV tri[3] = {poly[0], poly[k], poly[k + 1]};
        draw_one(w, tri, mat, fs);
    }
}

}}  // namespace evgx::raster
