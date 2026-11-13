// Public software rasterizer entry points: clear, draw a model, free scratch memory. The stages are in raster_*.cpp.
#include "raster.hpp"
#include "raster_internal.hpp"
#include "mem.hpp"

namespace evgx {
using namespace math;

namespace {
raster::SV* g_scratch = nullptr;       // per-vertex transform results, reused between calls
size_t g_scratch_cap = 0;
}

void raster_clear(Window* w, uint32_t argb) {
    size_t n = (size_t)w->fbw * w->fbh;
    for (size_t i = 0; i < n; i++) {
        w->color[i] = argb;
        w->depth[i] = 1.f;
    }
}

void raster_draw(Window* w, const Model* m, const mat4& world, const FrameState& fs) {
    if (!w->color || !w->depth || !m) return;
    mat4 vp = fs.proj * fs.view;
    for (int ii = 0; ii < m->ninst; ii++) {
        const Instance& in = m->inst[ii];
        const Primitive& pr = m->prims[in.prim];
        if (!pr.nverts || !pr.nidx) continue;
        const Material& mat = m->mats[(pr.material >= 0 && pr.material < m->nmats) ? pr.material : m->nmats - 1];
        mat4 model = world * in.world;
        mat4 nm = transpose(inverse(model));

        if (g_scratch_cap < pr.nverts) {
            mem_free(g_scratch);
            g_scratch_cap = pr.nverts;
            g_scratch = alloc_arr<raster::SV>(g_scratch_cap);
            if (!g_scratch) { g_scratch_cap = 0; return; }
        }
        // vertex stage: object -> world -> clip, normals to world space
        for (uint32_t i = 0; i < pr.nverts; i++) {
            vec4<float> p = model * vec4<float>(pr.pos[i], 1.f);
            raster::SV& s = g_scratch[i];
            s.wp = vec3<float>(p.x, p.y, p.z);
            s.clip = vp * p;
            if (pr.nrm) {
                vec4<float> n = nm * vec4<float>(pr.nrm[i], 0.f);
                s.n = vec3<float>(n.x, n.y, n.z);
            } else {
                s.n = vec3<float>(0.f, 0.f, 0.f);
            }
            s.col = pr.col ? pr.col[i] : vec3<float>(1.f, 1.f, 1.f);
        }
        // primitive stage: triangle list; flat normals when the mesh has none
        for (uint32_t t = 0; t + 2 < pr.nidx; t += 3) {
            uint32_t i0 = pr.idx[t], i1 = pr.idx[t + 1], i2 = pr.idx[t + 2];
            if (i0 >= pr.nverts || i1 >= pr.nverts || i2 >= pr.nverts) continue;
            raster::SV tri[3] = {g_scratch[i0], g_scratch[i1], g_scratch[i2]};
            if (!pr.nrm) {
                vec3<float> fn = cross(tri[1].wp - tri[0].wp, tri[2].wp - tri[0].wp);
                float l = length(fn);
                if (l < 1e-20f) continue;
                fn = fn * (1.f / l);
                tri[0].n = tri[1].n = tri[2].n = fn;
            }
            raster::clip_and_draw(w, tri, mat, fs);
        }
    }
}

void raster_free_scratch() {
    mem_free(g_scratch);
    g_scratch = nullptr;
    g_scratch_cap = 0;
}

}  // namespace evgx
