// Api::vulkan_draw and Api::vulkan_render_pixels: the public entry points for GPU mesh rendering.
// The work is in vk_render.cpp; this file only checks arguments and converts the matrix.
#include "../core/state.hpp"
#include "../core/platform.hpp"
#include "vk_render.hpp"
#include <evg/scene.hpp>

namespace evgx {
using namespace st;

namespace {
// column-major floats of a matrix, the layout the shaders read
void to_floats(const math::mat4& m, float out[16]) {
    const float* p = &m.c[0].x;
    for (int i = 0; i < 16; i++) out[i] = p[i];
}
}

bool Api::vulkan_draw(Window* w, const Model* m, const math::mat4& viewProj, float r, float g, float b) {
    if (!w || w->closed) return fail("window is closed");
    if (!m) return fail("no model to draw");
    if (w->external) plat::prepare_external(w->native);
    float vp[16];
    to_floats(viewProj, vp);
    const char* err = nullptr;
    if (!vkp::render_to_window(plat::app_instance(), w->native, (uint32_t)w->width, (uint32_t)w->height, m, vp, r, g, b, &err))
        return fail(err ? err : "vulkan draw failed");
    g_err[0] = 0;
    return true;
}

bool Api::vulkan_render_pixels(uint32_t width, uint32_t height, const Model* m, const math::mat4& viewProj, uint32_t* out,
                               float r, float g, float b) {
    if (!m || !out || !width || !height) return fail("bad arguments to vulkan_render_pixels");
    float vp[16];
    to_floats(viewProj, vp);
    const char* err = nullptr;
    if (!vkp::render_to_pixels(width, height, m, vp, r, g, b, out, &err))
        return fail(err ? err : "vulkan render failed");
    g_err[0] = 0;
    return true;
}

}  // namespace evgx
