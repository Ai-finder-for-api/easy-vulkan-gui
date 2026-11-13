// Api::vulkan_clear: the public entry point for GPU presentation. The work itself is in vk_present.cpp.
#include "../core/state.hpp"
#include "../core/platform.hpp"

namespace evgx {
using namespace st;

namespace vkp {
bool present_clear(void* hinst, void* hwnd, uint32_t w, uint32_t h, float r, float g, float b, const char** err);
}

bool Api::vulkan_clear(Window* w, float r, float g, float b) {
    if (!w || w->closed) return fail("window is closed");
    // host toolkits (Qt) can reset the window class after attach; re-apply each frame (cheap)
    if (w->external) plat::prepare_external(w->native);
    const char* err = nullptr;
    if (!vkp::present_clear(plat::app_instance(), w->native, (uint32_t)w->width, (uint32_t)w->height, r, g, b, &err))
        return fail(err ? err : "vulkan present failed");
    g_err[0] = 0;
    return true;
}

}  // namespace evgx
