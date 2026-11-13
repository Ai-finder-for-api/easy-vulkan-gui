// glTF scene graph: node transforms (matrix or TRS), a depth-limited walk that emits one instance per mesh placement, world bounds.
#include "glb_internal.hpp"
#include "mem.hpp"

namespace evgx { namespace glbi {
using math::vec3;
using math::vec4;
using math::mat4;
using math::quat;
using math::normalize;
using math::translate;
using math::scale;
using math::mat4_cast;
using math::min;
using math::max;

namespace {

// local transform of a node: an explicit matrix when present, otherwise translation * rotation * scale
mat4 node_local(Ctx& c, int node) {
    int mt = J(c, node, "matrix");
    if (mt >= 0 && json::count(c.d, mt) == 16) {
        mat4 m;
        for (int i = 0; i < 16; i++) m[i / 4][i % 4] = F(c, json::item(c.d, mt, i));
        return m;
    }
    vec3<float> t(0.f, 0.f, 0.f), s(1.f, 1.f, 1.f);
    quat q;
    int tt = J(c, node, "translation"), rr = J(c, node, "rotation"), ss = J(c, node, "scale");
    if (json::count(c.d, tt) == 3)
        t = vec3<float>(F(c, json::item(c.d, tt, 0)), F(c, json::item(c.d, tt, 1)), F(c, json::item(c.d, tt, 2)));
    if (json::count(c.d, ss) == 3)
        s = vec3<float>(F(c, json::item(c.d, ss, 0)), F(c, json::item(c.d, ss, 1)), F(c, json::item(c.d, ss, 2)));
    if (json::count(c.d, rr) == 4)   // glTF stores the quaternion as x, y, z, w
        q = normalize(quat(F(c, json::item(c.d, rr, 3)), F(c, json::item(c.d, rr, 0)), F(c, json::item(c.d, rr, 1)), F(c, json::item(c.d, rr, 2))));
    return translate(mat4(), t) * mat4_cast(q) * scale(mat4(), s);
}

void walk(Ctx& c, Build& b, int node_idx, const mat4& parent, int depth) {
    if (depth > 64) return;                              // guards against cyclic or absurdly deep hierarchies
    int node = json::item(c.d, c.nodes, node_idx);
    if (node < 0) return;
    mat4 world = parent * node_local(c, node);
    int mi = J(c, node, "mesh");
    if (mi >= 0) {
        int mesh = I(c, mi);
        if (mesh >= 0 && mesh < c.nmesh) {
            for (int k = 0; k < b.mesh_count[mesh]; k++) {
                int prim = b.mesh_first[mesh] + k;
                if (!b.m->prims[prim].nverts) continue;
                if (b.fill) { b.m->inst[b.fill_n].prim = prim; b.m->inst[b.fill_n].world = world; }
                b.fill_n++;
            }
        }
    }
    int ch = J(c, node, "children");
    for (int i = 0; i < json::count(c.d, ch); i++) walk(c, b, I(c, json::item(c.d, ch, i)), world, depth + 1);
}

}  // namespace

// two passes: count the instances, then fill them (so the array is allocated exactly once)
void build_instances(Ctx& c, Model* m, const int* mesh_first, const int* mesh_count, int nmesh, int nnodes) {
    Build b = {m, mesh_first, mesh_count, 0, false};
    int scene_idx = I(c, J(c, 0, "scene"), 0);
    int scene = json::item(c.d, J(c, 0, "scenes"), scene_idx);
    int roots = J(c, scene, "nodes");

    bool* is_child = alloc_arr<bool>((size_t)nnodes + 1);
    for (int i = 0; i < nnodes; i++) {
        int ch = J(c, json::item(c.d, c.nodes, i), "children");
        for (int k = 0; k < json::count(c.d, ch); k++) {
            int ci = I(c, json::item(c.d, ch, k));
            if (ci >= 0 && ci < nnodes) is_child[ci] = true;
        }
    }
    for (int pass = 0; pass < 2; pass++) {
        b.fill = pass == 1;
        b.fill_n = 0;
        if (roots >= 0) {
            for (int i = 0; i < json::count(c.d, roots); i++) walk(c, b, I(c, json::item(c.d, roots, i)), mat4(), 0);
        } else {                                         // no scene roots: start from every node that is not a child
            for (int i = 0; i < nnodes; i++) if (!is_child[i]) walk(c, b, i, mat4(), 0);
        }
        if (pass == 0) {
            m->ninst = b.fill_n;
            m->inst = alloc_arr<Instance>((size_t)(b.fill_n ? b.fill_n : 1));
            for (int i = 0; i < b.fill_n; i++) new (&m->inst[i]) Instance();
        }
    }
    mem_free(is_child);
}

// world-space bounds of all instances: the eight corners of each primitive's box, transformed
void compute_world_bounds(Model* m) {
    bool first = true;
    for (int i = 0; i < m->ninst; i++) {
        const Primitive& pr = m->prims[m->inst[i].prim];
        for (int cx = 0; cx < 8; cx++) {
            vec4<float> p = m->inst[i].world * vec4<float>((cx & 1) ? pr.bmax.x : pr.bmin.x, (cx & 2) ? pr.bmax.y : pr.bmin.y,
                                                          (cx & 4) ? pr.bmax.z : pr.bmin.z, 1.f);
            if (first) {
                m->bmin = m->bmax = vec3<float>(p.x, p.y, p.z);
                first = false;
            } else {
                m->bmin = vec3<float>(min(m->bmin.x, p.x), min(m->bmin.y, p.y), min(m->bmin.z, p.z));
                m->bmax = vec3<float>(max(m->bmax.x, p.x), max(m->bmax.y, p.y), max(m->bmax.z, p.z));
            }
        }
    }
}

}}  // namespace evgx::glbi
