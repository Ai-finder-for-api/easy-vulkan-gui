// glTF materials: PBR factors (base color, metallic, roughness, emissive, double-sided). Textures are not read yet.
#include "glb_internal.hpp"
#include "mem.hpp"

namespace evgx { namespace glbi {
using math::vec3;

void read_materials(Ctx& c, Model* m, int nmat) {
    // one extra entry at the end: the default material for primitives without a valid index
    m->nmats = nmat + 1;
    m->mats = alloc_arr<Material>((size_t)m->nmats);
    for (int i = 0; i < m->nmats; i++) new (&m->mats[i]) Material();
    m->mats[nmat].color = vec3<float>(0.8f, 0.8f, 0.8f);

    for (int i = 0; i < nmat; i++) {
        int mat = json::item(c.d, c.materials, i);
        int pbr = J(c, mat, "pbrMetallicRoughness");
        Material& o = m->mats[i];

        int bc = J(c, pbr, "baseColorFactor");
        if (json::count(c.d, bc) >= 3)
            o.color = vec3<float>(F(c, json::item(c.d, bc, 0)), F(c, json::item(c.d, bc, 1)), F(c, json::item(c.d, bc, 2)));
        o.metallic = F(c, J(c, pbr, "metallicFactor"), 1.0);
        o.roughness = F(c, J(c, pbr, "roughnessFactor"), 1.0);

        int em = J(c, mat, "emissiveFactor");
        if (json::count(c.d, em) == 3)
            o.emissive = vec3<float>(F(c, json::item(c.d, em, 0)), F(c, json::item(c.d, em, 1)), F(c, json::item(c.d, em, 2)));
        o.double_sided = json::boolean(c.d, J(c, mat, "doubleSided"));
    }
}

}}  // namespace evgx::glbi
