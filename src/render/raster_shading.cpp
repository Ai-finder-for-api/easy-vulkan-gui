// Fragment shading: Blinn-Phong with a directional light, or image-based light when an environment is set. Plus pixel packing.
#include "raster_internal.hpp"
#include <evg/lighting.hpp>

namespace evgx { namespace raster {
using namespace math;

uint32_t pack_argb(const vec3<float>& c) {
    float r = sqrt(clamp(c.x, 0.f, 1.f));
    float g = sqrt(clamp(c.y, 0.f, 1.f));
    float b = sqrt(clamp(c.z, 0.f, 1.f));
    return 0xFF000000u | ((uint32_t)(r * 255.f + 0.5f) << 16) | ((uint32_t)(g * 255.f + 0.5f) << 8) | (uint32_t)(b * 255.f + 0.5f);
}

vec3<float> shade_surface(const Material& mat, const FrameState& fs, const vec3<float>& N, const vec3<float>& V, const vec3<float>& col) {
    // Blinn-Phong terms (used by the analytic path)
    vec3<float> L = fs.light_to;
    vec3<float> Hh = normalize(L + V);
    float ndl = max(dot(N, L), 0.f);
    float ndh = max(dot(N, Hh), 0.f);
    int e2 = 2 + (int)((1.f - clamp(mat.roughness, 0.f, 1.f)) * 5.f);       // exponent 2^e2 (4..128)
    float spec = ndh;
    for (int i = 0; i < e2; i++) spec *= spec;
    if (ndl <= 0.f) spec = 0.f;

    vec3<float> lc = fs.light_color;
    float dk = 1.f - 0.6f * mat.metallic;                      // diffuse fraction (metals have none)
    float sk = 0.15f + 0.6f * mat.metallic;                    // specular strength
    vec3<float> albedo(mat.color.x * col.x, mat.color.y * col.y, mat.color.z * col.z);
    vec3<float> tint = mix_v(vec3<float>(1.f, 1.f, 1.f), albedo, mat.metallic);

    vec3<float> diffuse, specular;
    if (fs.env) {
        // image-based: SH irradiance (already /pi) for diffuse, prefiltered environment along R for specular
        vec3<float> R = N * (2.f * dot(N, V)) - V;
        diffuse = lighting_irradiance(*fs.env, N);
        specular = lighting_sample(*fs.env, normalize(R), mat.roughness);
        float fres = 0.04f + 0.96f * (1.f - max(dot(N, V), 0.f));
        float f5 = fres * fres * fres * fres * fres;
        specular = specular * (sk + (1.f - sk) * f5);
    } else {
        // analytic fallback: ambient + one directional light
        float amb = fs.ambient * (0.6f + 0.4f * N.y);
        diffuse = vec3<float>(amb + ndl * lc.x, amb + ndl * lc.y, amb + ndl * lc.z);
        specular = lc * spec * sk;
    }
    return albedo * diffuse * dk + specular * tint + mat.emissive;
}

}}  // namespace evgx::raster
