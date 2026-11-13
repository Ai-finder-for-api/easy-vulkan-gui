// Per-frame GPU presentation: wait for the slot, acquire, record a clear, submit, present. Returns without waiting on the GPU.
#include "vk_types.hpp"

namespace evgx { namespace vkp {

bool present_clear(void* hinst, void* hwnd, uint32_t w, uint32_t h, float r, float g, float b, const char** err) {
    *err = nullptr;
    if (!vk_init_context(err)) return false;
    WinSwap* s = vk_window_state(hinst, hwnd, w, h, err);
    if (!s) return false;

    // 1) the slot we reuse finished on the GPU two frames ago; wait only for it
    uint32_t slot = s->frame % kFramesInFlight;
    s->frame++;
    g_ctx.f.waitFences(g_ctx.dev, 1, &s->fence[slot], 1, ~0ull);
    g_ctx.f.resetFences(g_ctx.dev, 1, &s->fence[slot]);

    // 2) next image; the acquire signals this slot's semaphore
    uint32_t idx = 0;
    R res = g_ctx.f.acquire(g_ctx.dev, s->swap, ~0ull, s->semAcquire[slot], 0, &idx);
    if (res == RES_OUT_OF_DATE) {
        if (!vk_make_swapchain(*s, w, h, err)) return false;
        res = g_ctx.f.acquire(g_ctx.dev, s->swap, ~0ull, s->semAcquire[slot], 0, &idx);
    }
    if (res != 0 && res != RES_SUBOPTIMAL) { *err = "vkAcquireNextImageKHR failed"; return false; }
    if (idx >= s->n) { *err = "swapchain image index out of range"; return false; }

    // 3) record: UNDEFINED -> TRANSFER_DST, clear, TRANSFER_DST -> PRESENT_SRC
    Cmd cmd = g_ctx.cmd[slot];
    CommandBufferBeginInfo bi = {};
    bi.sType = S_COMMAND_BUFFER_BEGIN;
    bi.flags = 1;                                  // ONE_TIME_SUBMIT
    g_ctx.f.beginCmd(cmd, &bi);
    Range full = {ASPECT_COLOR, 0, 1, 0, 1};
    ImageMemoryBarrier toDst = {};
    toDst.sType = S_IMAGE_BARRIER;
    toDst.oldLayout = LAYOUT_UNDEFINED;
    toDst.newLayout = LAYOUT_TRANSFER_DST;
    toDst.srcQueueFamilyIndex = 0xFFFFFFFFu;
    toDst.dstQueueFamilyIndex = 0xFFFFFFFFu;
    toDst.dstAccessMask = ACCESS_TRANSFER_WRITE;
    toDst.image = s->imgs[idx];
    toDst.subresourceRange = full;
    g_ctx.f.barrier(cmd, STAGE_TOP, STAGE_TRANSFER, 0, 0, nullptr, 0, nullptr, 1, &toDst);
    float color[4] = {r, g, b, 1.0f};
    g_ctx.f.clear(cmd, s->imgs[idx], LAYOUT_TRANSFER_DST, color, 1, &full);
    ImageMemoryBarrier toPresent = toDst;
    toPresent.srcAccessMask = ACCESS_TRANSFER_WRITE;
    toPresent.dstAccessMask = 0;
    toPresent.oldLayout = LAYOUT_TRANSFER_DST;
    toPresent.newLayout = LAYOUT_PRESENT;
    g_ctx.f.barrier(cmd, STAGE_TRANSFER, STAGE_BOTTOM, 0, 0, nullptr, 0, nullptr, 1, &toPresent);
    g_ctx.f.endCmd(cmd);

    // 4) submit: wait for the image, signal this image's render semaphore and the slot fence
    uint32_t waitStage = STAGE_TRANSFER;
    SubmitInfo si = {};
    si.sType = S_SUBMIT;
    si.waitSemaphoreCount = 1;
    si.pWaitSemaphores = &s->semAcquire[slot];
    si.pWaitDstStageMask = &waitStage;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cmd;
    si.signalSemaphoreCount = 1;
    si.pSignalSemaphores = &s->semRender[idx];
    if (g_ctx.f.submit(g_ctx.q, 1, &si, s->fence[slot]) != 0) { *err = "vkQueueSubmit failed"; return false; }

    // 5) present after the render semaphore; no wait on the GPU here
    PresentInfo pi = {};
    pi.sType = S_PRESENT;
    pi.waitSemaphoreCount = 1;
    pi.pWaitSemaphores = &s->semRender[idx];
    pi.swapchainCount = 1;
    pi.pSwapchains = &s->swap;
    pi.pImageIndices = &idx;
    R pr = g_ctx.f.present(g_ctx.q, &pi);
    if (pr != 0 && pr != RES_SUBOPTIMAL && pr != RES_OUT_OF_DATE) { *err = "vkQueuePresentKHR failed"; return false; }
    return true;
}

const char* gpu_name() { return g_ctx.gpu; }
bool present_mailbox() { return g_ctx.presentMode == PRESENT_MAILBOX; }
uint32_t present_mode() { return g_ctx.presentMode; }

}}  // namespace evgx::vkp
