// Diffuse irradiance: order-2 spherical harmonics of the environment (Ramamoorthi & Hanrahan), and its evaluation per normal.
#include "env_internal.hpp"

namespace evgx { namespace env {
using namespace math;

void build_irradiance(Lighting* out, int w, int h) {
    // project radiance onto the 9 SH basis functions, weighting each texel by its solid angle
    vec3<float> L[9];
    for (int i = 0; i < 9; i++) L[i] = vec3<float>(0.f, 0.f, 0.f);
    double wsum = 0;
    for (int y = 0; y < h; y++) {
        float v = (y + 0.5f) / h, theta = v * pi;
        float dOmega = (2.f * pi / w) * (pi / h) * sin(theta);      // solid angle of the texel
        for (int x = 0; x < w; x++) {
            float u = (x + 0.5f) / w;
            vec3<float> d = uv_to_dir(u, v);
            vec3<float> c = fetch(out->rgb, w, h, x, y) * dOmega;
            float x_ = d.x, y_ = d.y, z_ = d.z;
            float b[9] = {
                0.282095f,
                0.488603f * y_, 0.488603f * z_, 0.488603f * x_,
                1.092548f * x_ * y_, 1.092548f * y_ * z_, 0.315392f * (3.f * z_ * z_ - 1.f),
                1.092548f * x_ * z_, 0.546274f * (x_ * x_ - y_ * y_)
            };
            for (int i = 0; i < 9; i++) L[i] += c * b[i];
            wsum += dOmega;
        }
    }
    // normalize so a uniform environment of radiance 1 gives irradiance pi; cosine-lobe factors A_l: pi, 2pi/3, pi/4
    float norm = wsum > 0 ? (float)(4.0 * pi / wsum) : 0.f;
    const float A[9] = {pi, 2.f * pi / 3.f, 2.f * pi / 3.f, 2.f * pi / 3.f, pi / 4.f, pi / 4.f, pi / 4.f, pi / 4.f, pi / 4.f};
    for (int i = 0; i < 9; i++) out->sh[i] = L[i] * (norm * A[i] / pi);   // stored divided by pi
}

}}  // namespace evgx::env

namespace evgx {
using namespace math;

vec3<float> lighting_irradiance(const Lighting& l, const vec3<float>& n) {
    const vec3<float>* s = l.sh;
    float b[9] = {
        0.282095f, 0.488603f * n.y, 0.488603f * n.z, 0.488603f * n.x,
        1.092548f * n.x * n.y, 1.092548f * n.y * n.z, 0.315392f * (3.f * n.z * n.z - 1.f),
        1.092548f * n.x * n.z, 0.546274f * (n.x * n.x - n.y * n.y)
    };
    vec3<float> e(0.f, 0.f, 0.f);
    for (int i = 0; i < 9; i++) e += s[i] * b[i];
    e.x = e.x < 0 ? 0 : e.x;
    e.y = e.y < 0 ? 0 : e.y;
    e.z = e.z < 0 ? 0 : e.z;
    return e;
}

}  // namespace evgx
