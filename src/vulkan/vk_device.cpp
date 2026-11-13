// Logical device, its queue, device-level entry points, and the command pool. vk_init_context runs all the setup steps once.
#include "vk_types.hpp"

namespace evgx { namespace vkp {

namespace {
const char* kDeviceExt[] = {"VK_KHR_swapchain"};
}

bool vk_create_device(const char** err) {
    float prio = 1.0f;
    DeviceQueueCreateInfo qci = {};
    qci.sType = S_DEVICE_QUEUE;
    qci.queueFamilyIndex = g_ctx.qf;
    qci.queueCount = 1;
    qci.pQueuePriorities = &prio;
    DeviceCreateInfo dci = {};
    dci.sType = S_DEVICE;
    dci.queueCreateInfoCount = 1;
    dci.pQueueCreateInfos = &qci;
    dci.enabledExtensionCount = 1;
    dci.ppEnabledExtensionNames = kDeviceExt;
    if (g_ctx.f.createDevice(g_ctx.pd, &dci, nullptr, &g_ctx.dev) != 0 || !g_ctx.dev) { *err = "vkCreateDevice failed"; return false; }

    Dev dev = g_ctx.dev;
    auto get = g_ctx.getDevProc;
    g_ctx.f.destroyDevice = (PFN_vkDestroyDevice)get(dev, "vkDestroyDevice");
    g_ctx.f.getQueue = (PFN_vkGetDeviceQueue)get(dev, "vkGetDeviceQueue");
    g_ctx.f.createSwapchain = (PFN_vkCreateSwapchainKHR)get(dev, "vkCreateSwapchainKHR");
    g_ctx.f.destroySwapchain = (PFN_vkDestroySwapchainKHR)get(dev, "vkDestroySwapchainKHR");
    g_ctx.f.swapImages = (PFN_vkGetSwapchainImagesKHR)get(dev, "vkGetSwapchainImagesKHR");
    g_ctx.f.acquire = (PFN_vkAcquireNextImageKHR)get(dev, "vkAcquireNextImageKHR");
    g_ctx.f.present = (PFN_vkQueuePresentKHR)get(dev, "vkQueuePresentKHR");
    g_ctx.f.createSemaphore = (PFN_vkCreateSemaphore)get(dev, "vkCreateSemaphore");
    g_ctx.f.destroySemaphore = (PFN_vkDestroySemaphore)get(dev, "vkDestroySemaphore");
    g_ctx.f.createFence = (PFN_vkCreateFence)get(dev, "vkCreateFence");
    g_ctx.f.destroyFence = (PFN_vkDestroyFence)get(dev, "vkDestroyFence");
    g_ctx.f.waitFences = (PFN_vkWaitForFences)get(dev, "vkWaitForFences");
    g_ctx.f.resetFences = (PFN_vkResetFences)get(dev, "vkResetFences");
    g_ctx.f.createPool = (PFN_vkCreateCommandPool)get(dev, "vkCreateCommandPool");
    g_ctx.f.destroyPool = (PFN_vkDestroyCommandPool)get(dev, "vkDestroyCommandPool");
    g_ctx.f.allocCmd = (PFN_vkAllocateCommandBuffers)get(dev, "vkAllocateCommandBuffers");
    g_ctx.f.beginCmd = (PFN_vkBeginCommandBuffer)get(dev, "vkBeginCommandBuffer");
    g_ctx.f.endCmd = (PFN_vkEndCommandBuffer)get(dev, "vkEndCommandBuffer");
    g_ctx.f.barrier = (PFN_vkCmdPipelineBarrier)get(dev, "vkCmdPipelineBarrier");
    g_ctx.f.clear = (PFN_vkCmdClearColorImage)get(dev, "vkCmdClearColorImage");
    g_ctx.f.submit = (PFN_vkQueueSubmit)get(dev, "vkQueueSubmit");
    g_ctx.f.deviceWaitIdle = (PFN_vkDeviceWaitIdle)get(dev, "vkDeviceWaitIdle");
    g_ctx.f.getQueue(dev, g_ctx.qf, 0, &g_ctx.q);
    return true;
}

// one-time setup: instance -> device -> queue -> command pool and per-slot command buffers
bool vk_init_context(const char** err) {
    if (g_ctx.ready) return true;
    if (!vk_create_instance(err)) return false;
    if (!vk_pick_device(err)) return false;
    if (!vk_create_device(err)) return false;

    CommandPoolCreateInfo pci = {};
    pci.sType = S_COMMAND_POOL;
    pci.flags = POOL_RESET;                       // each slot's buffer resets implicitly when begun again
    pci.queueFamilyIndex = g_ctx.qf;
    if (g_ctx.f.createPool(g_ctx.dev, &pci, nullptr, &g_ctx.pool) != 0) { *err = "vkCreateCommandPool failed"; return false; }
    CommandBufferAllocateInfo cbai = {};
    cbai.sType = S_COMMAND_BUFFER_ALLOC;
    cbai.commandPool = g_ctx.pool;
    cbai.commandBufferCount = kFramesInFlight;
    if (g_ctx.f.allocCmd(g_ctx.dev, &cbai, g_ctx.cmd) != 0) { *err = "vkAllocateCommandBuffers failed"; return false; }
    g_ctx.ready = true;
    return true;
}

}}  // namespace evgx::vkp
