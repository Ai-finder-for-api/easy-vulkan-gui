// Per-window synchronization: fences (start signaled), acquire semaphores per frame slot, render semaphores per image.
#include "vk_types.hpp"

namespace evgx { namespace vkp {

bool vk_make_sync(WinSwap& s, const char** err) {
    SemaphoreCreateInfo sc = {};
    sc.sType = S_SEMAPHORE;
    FenceCreateInfo fc = {};
    fc.sType = S_FENCE;
    fc.flags = FENCE_SIGNALED;                    // first use of a slot must not wait
    for (int i = 0; i < kFramesInFlight; i++) {
        if (g_ctx.f.createSemaphore(g_ctx.dev, &sc, nullptr, &s.semAcquire[i]) != 0) { *err = "vkCreateSemaphore failed"; return false; }
        if (g_ctx.f.createFence(g_ctx.dev, &fc, nullptr, &s.fence[i]) != 0) { *err = "vkCreateFence failed"; return false; }
    }
    for (int i = 0; i < kMaxImages; i++)
        if (g_ctx.f.createSemaphore(g_ctx.dev, &sc, nullptr, &s.semRender[i]) != 0) { *err = "vkCreateSemaphore failed"; return false; }
    s.sync_ready = true;
    return true;
}

}}  // namespace evgx::vkp
