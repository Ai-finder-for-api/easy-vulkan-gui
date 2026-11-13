// Internal GLB loader declarations shared by render/glb_*.cpp. Not part of the public API.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <evg/scene.hpp>
#include "json.hpp"

// placement new without <new> (the library does not include the C++ standard library headers)
inline void* operator new(size_t, void* p) noexcept { return p; }

namespace evgx { namespace glbi {

using math::vec3;
using math::vec4;
using math::mat4;

// everything one parse needs: the JSON document, the binary chunk, and the top-level array indices
struct Ctx {
    json::Doc d;
    const uint8_t* bin;
    size_t binlen;
    const char* err;
    int accessors, bufferViews, meshes, materials, nodes;
    int nmesh;
};

// a resolved accessor: where its data is, what it holds
struct Acc { int ctype; bool norm; uint32_t count; int ncomp; size_t off, stride; };

// little-endian reads from the binary chunk
uint32_t rd32(const uint8_t* p);
uint32_t rd16(const uint8_t* p);
float rdf(const uint8_t* p);

// JSON helpers: untrusted numbers are converted without out-of-range casts (those are undefined behavior)
int J(Ctx& c, int obj, const char* key);
int I(Ctx& c, int tok, int def = 0);
size_t Z(Ctx& c, int tok, double maxv = 4294967296.0);
float F(Ctx& c, int tok, double def = 0);

// accessor.cpp
bool locate(Ctx& c, int ai, Acc& a);
float comp(const Acc& a, const uint8_t* p);
vec3<float>* read_vec3(Ctx& c, int ai, uint32_t* count);
uint32_t* read_indices(Ctx& c, int ai, uint32_t* count);

// material.cpp
void read_materials(Ctx& c, Model* m, int nmat);

// mesh.cpp: primitives of every mesh, with bounds
bool read_primitives(Ctx& c, Model* m, const int* mesh_first, const int* mesh_count, int nmesh, int nmat);

// scene.cpp: node hierarchy -> flat instances, world bounds
struct Build { Model* m; const int* mesh_first; const int* mesh_count; int fill_n; bool fill; };
void build_instances(Ctx& c, Model* m, const int* mesh_first, const int* mesh_count, int nmesh, int nnodes);
void compute_world_bounds(Model* m);

}}  // namespace evgx::glbi
