// Environment lighting (image-based). Core API: evg.import_lighting("sky.exr") (see evg_exr module for EXR decoding).
#pragma once
#include <stdint.h>
#include "math.hpp"

namespace evgx {

// Equirectangular HDR environment (linear RGB float). Built by the EXR module, or from raw float data.
struct Lighting {
    int w = 0, h = 0;
    float* rgb = nullptr;              // w*h*3, linear, row 0 = top (+Y)
    // Precomputed irradiance (diffuse, 9 spherical-harmonic coefficients per channel), filled by evg.import_lighting
    math::vec3<float> sh[9];
    // Prefiltered roughness levels for specular: 6 mips, mip i has roughness i/5 (box-filtered, approximate)
    int nmips = 0;
    int mw[6], mh[6];
    float* mip[6];
};

// Build lighting from linear float RGB data (takes ownership semantics: copies the input).
bool lighting_build(Lighting* out, const float* rgb, int w, int h);
void lighting_free(Lighting* l);

// Sample the environment in direction d (unit), roughness in [0,1] selects the prefiltered level.
math::vec3<float> lighting_sample(const Lighting& l, const math::vec3<float>& d, float roughness);
// Irradiance (already cosine-convolved) for a surface normal n, divided by pi (so albedo * irradiance is the diffuse term).
math::vec3<float> lighting_irradiance(const Lighting& l, const math::vec3<float>& n);

} // namespace evgx
