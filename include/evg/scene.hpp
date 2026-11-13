// Scene data produced by the GLB loader. Plain structs, no STL.
#pragma once
#include <stdint.h>
#include "math.hpp"

namespace evgx {

struct Material {
    math::vec3<float> color = math::vec3<float>(1.f, 1.f, 1.f);      // baseColorFactor.rgb
    math::vec3<float> emissive = math::vec3<float>(0.f, 0.f, 0.f);
    float metallic = 0.f, roughness = 1.f;
    bool  double_sided = false;
};

struct Primitive {
    uint32_t nverts = 0, nidx = 0;
    math::vec3<float>* pos = nullptr;
    math::vec3<float>* nrm = nullptr;   // null => flat shading from face normals
    math::vec3<float>* col = nullptr;   // COLOR_0 rgb, null => white
    uint32_t* idx = nullptr;            // triangle list
    int material = -1;                  // index into Model::mats
    math::vec3<float> bmin, bmax;       // local-space bounds
};

struct Instance { int prim; math::mat4 world; };   // one drawable primitive placed in the scene graph

struct Model {
    Primitive* prims = nullptr; int nprims = 0;
    Material*  mats = nullptr;  int nmats = 0;      // mats[nmats-1] is the default material
    Instance*  inst = nullptr;  int ninst = 0;
    math::vec3<float> bmin, bmax;                   // world-space bounds of all instances
    int skipped = 0;                                // primitives skipped (non-triangle modes, missing POSITION)
};

} // namespace evgx
