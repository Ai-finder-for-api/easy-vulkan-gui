// Internal environment-map helpers shared by render/env_*.cpp. Not part of the public API.
#pragma once
#include <evg/lighting.hpp>

namespace evgx { namespace env {

using math::vec3;

// equirectangular mapping: direction <-> (u, v) with v = 0 at +Y
void dir_to_uv(const vec3<float>& d, float* u, float* v);
vec3<float> uv_to_dir(float u, float v);

// clamped texel read (for edges) and bilinear filtering with horizontal wrap
vec3<float> fetch(const float* img, int w, int h, int x, int y);
vec3<float> bilinear(const float* img, int w, int h, float u, float v);

// half-resolution copy by 2x2 averaging; returns a new buffer (caller frees), null on allocation failure
float* halve(const float* src, int w, int h, int* ow, int* oh);

// order-2 spherical-harmonic irradiance, stored divided by pi
void build_irradiance(Lighting* out, int w, int h);

// roughness-prefiltered mip chain (mip 0 is the full-resolution copy)
void build_mips(Lighting* out);

}}  // namespace evgx::env
