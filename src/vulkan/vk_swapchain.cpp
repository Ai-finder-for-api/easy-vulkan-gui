// Swapchains: size and format selection, creation and re-creation, and the present mode (mailbox > immediate > FIFO).
#include "vk_types.hpp"

namespace evgx { namespace vkp {

// creates the swapchain of one window (or re-creates it for a new size, retiring the old one)
bool vk_make_swapchain(WinSwap& s, uint32_t w, uint32_t h, const char** err) {
    SurfaceCapabilities caps = {};
    if (g_ctx.f.surfaceCaps(g_ctx.pd, s.surface, &caps) != 0) { *err = "surface capabilities failed"; return false; }

    // extent: the surface's size when it has one, else the window's requested size, clamped to the allowed range
    Ext2D e = caps.currentExtent;
    if (e.width == 0xFFFFFFFFu) { e.width = w; e.height = h; }
    if (e.width < caps.minImageExtent.width) e.width = caps.minImageExtent.width;
    if (e.height < caps.minImageExtent.height) e.height = caps.minImageExtent.height;
    if (caps.maxImageExtent.width && e.width > caps.maxImageExtent.width) e.width = caps.maxImageExtent.width;
    if (caps.maxImageExtent.height && e.height > caps.maxImageExtent.height) e.height = caps.maxImageExtent.height;
    if (!(caps.supportedUsageFlags & USAGE_TRANSFER_DST)) { *err = "surface does not allow transfer-dst usage"; return false; }

    // format: BGRA8 UNORM when offered, else the first format the surface lists
    uint32_t nfmt = 16;
    SurfaceFormat fm[16];
    g_ctx.f.surfaceFormats(g_ctx.pd, s.surface, &nfmt, fm);
    s.format = fm[0].format;
    s.colorSpace = fm[0].colorSpace;
    for (uint32_t i = 0; i < nfmt; i++)
        if (fm[i].format == FORMAT_BGRA8_UNORM) { s.format = fm[i].format; s.colorSpace = fm[i].colorSpace; break; }

    // opaque alpha when available; otherwise the lowest supported composite-alpha bit
    s.alpha = (caps.supportedCompositeAlpha & ALPHA_OPAQUE) ? ALPHA_OPAQUE : caps.supportedCompositeAlpha & (~caps.supportedCompositeAlpha + 1);

    uint32_t n = caps.minImageCount + 1;
    if (caps.maxImageCount && n > caps.maxImageCount) n = caps.maxImageCount;
    if (n > kMaxImages) n = kMaxImages;

    SwapchainCreateInfo sci = {};
    sci.sType = S_SWAPCHAIN;
    sci.surface = s.surface;
    sci.minImageCount = n;
    sci.imageFormat = (int32_t)s.format;
    sci.imageColorSpace = (int32_t)s.colorSpace;
    sci.imageExtent = e;
    sci.imageArrayLayers = 1;
    sci.imageUsage = USAGE_TRANSFER_DST;
    sci.imageSharingMode = 0;
    sci.preTransform = caps.currentTransform;
    sci.compositeAlpha = s.alpha;
    sci.presentMode = g_ctx.presentMode;
    sci.clipped = 1;
    sci.oldSwapchain = s.swap;

    H newSwap = 0;
    if (g_ctx.f.createSwapchain(g_ctx.dev, &sci, nullptr, &newSwap) != 0 || !newSwap) { *err = "vkCreateSwapchainKHR failed"; return false; }
    if (s.swap) {                                  // old images may still be in flight: drain before retiring them
        g_ctx.f.deviceWaitIdle(g_ctx.dev);
        g_ctx.f.destroySwapchain(g_ctx.dev, s.swap, nullptr);
    }
    s.swap = newSwap;
    s.w = e.width;
    s.h = e.height;
    uint32_t cnt = kMaxImages;
    g_ctx.f.swapImages(g_ctx.dev, s.swap, &cnt, s.imgs);
    s.n = cnt;
    return true;
}

// mailbox (no tearing, no waiting on the display) > immediate (no waiting, may tear) > FIFO (vsync-paced)
void vk_choose_present_mode(H surface) {
    uint32_t n = 8;
    uint32_t modes[8] = {};
    g_ctx.presentMode = PRESENT_FIFO;
    if (g_ctx.f.presentModes(g_ctx.pd, surface, &n, modes) != 0) return;
    bool mailbox = false, immediate = false;
    for (uint32_t i = 0; i < n && i < 8; i++) {
        if (modes[i] == PRESENT_MAILBOX) mailbox = true;
        if (modes[i] == PRESENT_IMMEDIATE) immediate = true;
    }
    if (mailbox) g_ctx.presentMode = PRESENT_MAILBOX;
    else if (immediate) g_ctx.presentMode = PRESENT_IMMEDIATE;
    g_ctx.modeCount = n;
    for (uint32_t i = 0; i < n && i < 8; i++) g_ctx.modes[i] = modes[i];
}

}}  // namespace evgx::vkp
