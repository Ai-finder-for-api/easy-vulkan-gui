// glTF mesh primitives: positions, normals, colors, indices, material index and local bounds. Triangle lists only.
#include "glb_internal.hpp"
#include "mem.hpp"

namespace evgx { namespace glbi {
using math::vec3;
using math::min;
using math::max;

// reads every primitive of every mesh into m->prims; unsupported modes are counted in m->skipped
bool read_primitives(Ctx& c, Model* m, const int* mesh_first, const int* mesh_count, int nmesh, int nmat) {
    int total = 0;
    for (int i = 0; i < nmesh; i++) total += mesh_count[i];
    m->nprims = total;
    m->prims = alloc_arr<Primitive>((size_t)(total ? total : 1));
    if (!m->prims) { c.err = "out of memory"; return false; }
    for (int i = 0; i < total; i++) new (&m->prims[i]) Primitive();

    for (int mi = 0; mi < nmesh; mi++) {
        int prims = J(c, json::item(c.d, c.meshes, mi), "primitives");
        for (int k = 0; k < mesh_count[mi]; k++) {
            int pj = json::item(c.d, prims, k);
            Primitive& pr = m->prims[mesh_first[mi] + k];
            int mode = I(c, J(c, pj, "mode"), 4.0);
            int attr = J(c, pj, "attributes");
            int pos = J(c, attr, "POSITION");
            if (mode != 4 || pos < 0) { m->skipped++; continue; }      // only triangle lists are supported

            uint32_t n = 0;
            pr.pos = read_vec3(c, I(c, pos), &n);
            if (!pr.pos) return false;
            pr.nverts = n;

            uint32_t cn = 0;
            int nr = J(c, attr, "NORMAL");
            if (nr >= 0) {
                pr.nrm = read_vec3(c, I(c, nr), &cn);
                if (!pr.nrm) return false;
                if (cn != n) { c.err = "NORMAL count mismatch"; return false; }
            }
            int co = J(c, attr, "COLOR_0");
            if (co >= 0) {
                pr.col = read_vec3(c, I(c, co), &cn);
                if (!pr.col) return false;
                if (cn != n) { c.err = "COLOR_0 count mismatch"; return false; }
            }
            int ix = J(c, pj, "indices");
            if (ix >= 0) {
                pr.idx = read_indices(c, I(c, ix), &pr.nidx);
                if (!pr.idx) return false;
            } else {                                                  // no index buffer: vertices are already in order
                pr.idx = alloc_arr<uint32_t>(n ? n : 1);
                if (!pr.idx) { c.err = "out of memory"; return false; }
                for (uint32_t i = 0; i < n; i++) pr.idx[i] = i;
                pr.nidx = n;
            }
            pr.nidx -= pr.nidx % 3;                                   // whole triangles only

            int mt = J(c, pj, "material");
            pr.material = (mt >= 0 && I(c, mt) < nmat) ? I(c, mt) : nmat;

            pr.bmin = pr.bmax = n ? pr.pos[0] : vec3<float>(0.f, 0.f, 0.f);
            for (uint32_t i = 1; i < n; i++) {
                const vec3<float>& v = pr.pos[i];
                pr.bmin = vec3<float>(min(pr.bmin.x, v.x), min(pr.bmin.y, v.y), min(pr.bmin.z, v.z));
                pr.bmax = vec3<float>(max(pr.bmax.x, v.x), max(pr.bmax.y, v.y), max(pr.bmax.z, v.z));
            }
        }
    }
    return true;
}

}}  // namespace evgx::glbi
