// Software frame API: begin/camera/light/draw/end. Vulkan presentation is in src/vulkan.
#include "state.hpp"
#include "platform.hpp"

namespace evgx {
using namespace math;
using namespace st;

bool Api::begin(Window* w, float r, float g, float b) {
    if (!w || w->closed || !fb_ensure(w)) { g_cur = nullptr; return false; }
    g_cur = w;
    g_have_cam = false;
    g_fs.view = mat4(); g_fs.proj = mat4();
    g_fs.cam_pos = vec3<float>(0.f, 0.f, 0.f);
    g_fs.light_to = normalize(vec3<float>(0.4f, 0.8f, 0.5f));
    g_fs.light_color = vec3<float>(1.f, 1.f, 1.f);
    g_fs.ambient = 0.28f;
    g_fs.env = g_env_on ? &g_env : nullptr;
    auto q = [](float v) { v = clamp(v, 0.f, 1.f); return (uint32_t)(v * 255.f + 0.5f); };
    raster_clear(w, 0xFF000000u | (q(r) << 16) | (q(g) << 8) | q(b));
    return true;
}

void Api::view_proj(const mat4& view, const mat4& proj) {
    g_fs.view = view;
    g_fs.proj = proj;
    mat4 inv = inverse(view);
    g_fs.cam_pos = vec3<float>(inv[3].x, inv[3].y, inv[3].z);
    g_have_cam = true;
}

void Api::camera(const Camera& c) {
    if (!g_cur) return;
    float aspect = (float)g_cur->fbw / (float)g_cur->fbh;
    view_proj(c.view(), c.proj(aspect));
}

void Api::light(const vec3<float>& direction, const vec3<float>& color, float ambient) {
    g_fs.light_to = normalize(-direction);
    g_fs.light_color = color;
    g_fs.ambient = ambient;
}

void Api::draw(const Model* m, const mat4& world) {
    if (!g_cur || !m) return;
    if (!g_have_cam) { Camera c; camera(c); }
    raster_draw(g_cur, m, world, g_fs);
}

void Api::end() {
    if (g_cur) plat::present(g_cur);
    g_cur = nullptr;
}

}  // namespace evgx
