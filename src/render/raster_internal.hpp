// Internal rasterizer declarations shared by the render/raster_*.cpp files. Not part of the public API.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include "raster.hpp"

namespace evgx { namespace raster {

using math::vec3;
using math::vec4;

// a vertex after the world/view/projection transforms, with the attributes that are interpolated across a triangle
struct SV { vec4<float> clip; vec3<float> n, wp, col; };

// per-triangle data computed once before the fill loop
struct TriSetup {
    float sx[3], sy[3], sz[3], iw[3];   // screen x, y, depth in [0,1], and 1/w per vertex
    float A[3], B[3], C[3];             // edge functions: barycentric weight k = A*px + B*py + C
    int x0, x1, y0, y1;                 // clamped pixel bounding box
    bool back;                          // true for back faces (seen from behind)
};

SV lerp_sv(const SV& a, const SV& b, float t);

// computes TriSetup for three clip-space vertices; false when the triangle is degenerate or off-screen
bool tri_setup(const Window* w, const SV* v, const Material& mat, TriSetup& out);

// writes shaded pixels for a set-up triangle (depth-tested, perspective-correct)
void tri_fill(Window* w, const SV* v, const TriSetup& t, const Material& mat, const FrameState& fs);

// Blinn-Phong or image-based shading at one fragment; returns linear RGB (before gamma)
vec3<float> shade_surface(const Material& mat, const FrameState& fs, const vec3<float>& N, const vec3<float>& V, const vec3<float>& col);

// packs linear RGB into an opaque ARGB pixel (gamma ~2.0)
uint32_t pack_argb(const vec3<float>& c);

// clips a clip-space triangle against the near/far/side planes and rasterizes what remains
void clip_and_draw(Window* w, const SV* t, const Material& mat, const FrameState& fs);

}}  // namespace evgx::raster
