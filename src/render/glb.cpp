// GLB (binary glTF 2.0) entry points: container parsing, then materials, primitives, scene graph and bounds.
#include "glb.hpp"
#include "glb_internal.hpp"
#include "mem.hpp"

namespace evgx {
using namespace glbi;

void model_free(Model* m) {
    if (!m) return;
    for (int i = 0; i < m->nprims; i++) {
        mem_free(m->prims[i].pos);
        mem_free(m->prims[i].nrm);
        mem_free(m->prims[i].col);
        mem_free(m->prims[i].idx);
    }
    mem_free(m->prims);
    mem_free(m->mats);
    mem_free(m->inst);
    mem_free(m);
}

Model* glb_parse(const uint8_t* data, size_t size, const char** err) {
    const char* dummy = nullptr;
    if (!err) err = &dummy;
    *err = nullptr;

    // 1) container: 12-byte header, then JSON and BIN chunks
    if (size < 20 || rd32(data) != 0x46546C67u) { *err = "not a GLB file (bad magic)"; return nullptr; }
    if (rd32(data + 4) != 2) { *err = "unsupported GLB version"; return nullptr; }
    size_t total = rd32(data + 8);
    if (total > size) total = size;
    const uint8_t* jsn = nullptr; size_t jlen = 0;
    const uint8_t* bin = nullptr; size_t blen = 0;
    for (size_t off = 12; off + 8 <= total;) {
        size_t clen = rd32(data + off);
        uint32_t ctype = rd32(data + off + 4);
        if (off + 8 + clen > total) { *err = "truncated GLB chunk"; return nullptr; }
        if (ctype == 0x4E4F534Au && !jsn) { jsn = data + off + 8; jlen = clen; }
        else if (ctype == 0x004E4942u && !bin) { bin = data + off + 8; blen = clen; }
        off += 8 + ((clen + 3) & ~(size_t)3);
    }
    if (!jsn) { *err = "GLB has no JSON chunk"; return nullptr; }

    // 2) JSON document
    int cap = (int)(jlen / 2 + 8);
    json::Tok* toks = alloc_arr<json::Tok>((size_t)cap);
    if (!toks) { *err = "out of memory"; return nullptr; }
    Ctx c = {};
    if (!json::parse((const char*)jsn, (int)jlen, toks, cap, &c.d)) { mem_free(toks); *err = "invalid glTF JSON"; return nullptr; }
    c.bin = bin; c.binlen = blen; c.err = nullptr;
    c.accessors = J(c, 0, "accessors");
    c.bufferViews = J(c, 0, "bufferViews");
    c.meshes = J(c, 0, "meshes");
    c.materials = J(c, 0, "materials");
    c.nodes = J(c, 0, "nodes");
    c.nmesh = json::count(c.d, c.meshes);
    int nmesh = c.nmesh;
    int nmat = json::count(c.d, c.materials);
    int nnodes = json::count(c.d, c.nodes);

    Model* m = alloc_arr<Model>(1);
    int* mesh_first = alloc_arr<int>((size_t)nmesh + 1);
    int* mesh_count = alloc_arr<int>((size_t)nmesh + 1);
    if (!m || !mesh_first || !mesh_count) {
        mem_free(toks); mem_free(mesh_first); mem_free(mesh_count); mem_free(m);
        *err = "out of memory";
        return nullptr;
    }
    new (m) Model();

    // 3) materials, then primitives (with their mesh index ranges)
    read_materials(c, m, nmat);
    int total_prims = 0;
    for (int i = 0; i < nmesh; i++) {
        mesh_first[i] = total_prims;
        mesh_count[i] = json::count(c.d, J(c, json::item(c.d, c.meshes, i), "primitives"));
        total_prims += mesh_count[i];
    }
    if (!read_primitives(c, m, mesh_first, mesh_count, nmesh, nmat)) {
        *err = c.err ? c.err : "failed to read mesh data";
        model_free(m); mem_free(mesh_first); mem_free(mesh_count); mem_free(toks);
        return nullptr;
    }

    // 4) scene graph -> instances, then bounds
    build_instances(c, m, mesh_first, mesh_count, nmesh, nnodes);
    if (!m->ninst) {
        *err = "model has no renderable triangle meshes";
        model_free(m); mem_free(mesh_first); mem_free(mesh_count); mem_free(toks);
        return nullptr;
    }
    compute_world_bounds(m);

    mem_free(mesh_first);
    mem_free(mesh_count);
    mem_free(toks);
    return m;
}

}  // namespace evgx
