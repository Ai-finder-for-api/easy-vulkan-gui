// Per-window Vulkan state: finds or creates a window's surface, sync objects and swapchain; resizes swapchains.
#include "vk_types.hpp"

namespace evgx { namespace vkp {

WinSwap* vk_window_state(void* hinst, void* hwnd, uint32_t w, uint32_t h, const char** err) {
    for (int i = 0; i < g_nwins; i++) {
        if (g_wins[i].hwnd != hwnd) continue;
        WinSwap* s = &g_wins[i];
        if (s->w != w || s->h != h) {                // window was resized: rebuild its swapchain
            if (!vk_make_swapchain(*s, w, h, err)) return nullptr;
        }
        return s;
    }

    if (g_nwins >= kMaxWindows) { *err = "too many Vulkan windows"; return nullptr; }
    WinSwap* s = &g_wins[g_nwins++];
    s->hwnd = hwnd;
    Win32SurfaceCreateInfo wci = {};
    wci.sType = S_WIN32_SURFACE;
    wci.hinstance = hinst;
    wci.hwnd = hwnd;
    if (g_ctx.f.createSurface(g_ctx.inst, &wci, nullptr, &s->surface) != 0 || !s->surface) {
        *err = "vkCreateWin32SurfaceKHR failed";
        return nullptr;
    }
    uint32_t supported = 0;
    g_ctx.f.surfaceSupport(g_ctx.pd, g_ctx.qf, s->surface, &supported);
    if (!supported) { *err = "GPU queue cannot present to this window"; return nullptr; }
    vk_choose_present_mode(s->surface);
    if (!vk_make_sync(*s, err)) return nullptr;
    if (!vk_make_swapchain(*s, w, h, err)) return nullptr;
    return s;
}

}}  // namespace evgx::vkp
