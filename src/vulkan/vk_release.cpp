// GPU cleanup: frees one window's Vulkan objects (sync objects, swapchain, surface). When the last window goes,
// the command pool, device and instance are destroyed too, so a later window starts from a clean context.
#include "vk_types.hpp"

namespace evgx { namespace vkp {

namespace {

// destroys everything a window owns; the device must be idle first (the caller waits)
void destroy_window_objects(WinSwap& s) {
    if (s.swap) g_ctx.f.destroySwapchain(g_ctx.dev, s.swap, nullptr);
    s.swap = 0;
    for (int i = 0; i < kFramesInFlight; i++) {
        if (s.fence[i]) g_ctx.f.destroyFence(g_ctx.dev, s.fence[i], nullptr);
        if (s.semAcquire[i]) g_ctx.f.destroySemaphore(g_ctx.dev, s.semAcquire[i], nullptr);
        s.fence[i] = 0;
        s.semAcquire[i] = 0;
    }
    for (int i = 0; i < kMaxImages; i++) {
        if (s.semRender[i]) g_ctx.f.destroySemaphore(g_ctx.dev, s.semRender[i], nullptr);
        s.semRender[i] = 0;
    }
    if (s.surface) g_ctx.f.destroySurface(g_ctx.inst, s.surface, nullptr);
    s.surface = 0;
    s.sync_ready = false;
}

}  // namespace

// destroys the shared device-level objects and the instance; resets the context so vk_init_context can run again
void vk_shutdown() {
    if (!g_ctx.ready) return;
    if (g_ctx.dev) {
        g_ctx.f.deviceWaitIdle(g_ctx.dev);
        if (g_ctx.pool && g_ctx.f.destroyPool) g_ctx.f.destroyPool(g_ctx.dev, g_ctx.pool, nullptr);
        if (g_ctx.f.destroyDevice) g_ctx.f.destroyDevice(g_ctx.dev, nullptr);
    }
    if (g_ctx.inst && g_ctx.f.destroyInstance) g_ctx.f.destroyInstance(g_ctx.inst, nullptr);

    g_ctx.pool = 0;
    g_ctx.dev = nullptr;
    g_ctx.inst = nullptr;
    g_ctx.pd = nullptr;
    g_ctx.q = nullptr;
    g_ctx.qf = 0;
    for (int i = 0; i < kFramesInFlight; i++) g_ctx.cmd[i] = nullptr;
    g_ctx.f = Ptrs();
    g_ctx.gipa = nullptr;
    g_ctx.getDevProc = nullptr;
    g_ctx.ready = false;
}

// frees the window registered under this native handle, if any. Unknown handles are ignored.
void vk_release_window(void* hwnd) {
    if (!hwnd) return;
    for (int i = 0; i < g_nwins; i++) {
        if (g_wins[i].hwnd != hwnd) continue;
        if (g_ctx.ready) {
            g_ctx.f.deviceWaitIdle(g_ctx.dev);            // no frame may still be using these objects
            destroy_window_objects(g_wins[i]);
        }
        for (int k = i; k + 1 < g_nwins; k++) g_wins[k] = g_wins[k + 1];   // keep the table dense
        g_nwins--;
        g_wins[g_nwins] = WinSwap();
        break;
    }
    if (g_nwins == 0) vk_shutdown();
}

}}  // namespace evgx::vkp
