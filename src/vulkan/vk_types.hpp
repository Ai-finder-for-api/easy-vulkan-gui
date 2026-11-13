// Shared Vulkan types, function pointers and state for the presentation backend. Internal to src/vulkan.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <evg/vk_min.h>
#include "../core/platform.hpp"

namespace evgx { namespace vkp {

typedef int32_t R;                       // VkResult
typedef uint64_t H;                      // non-dispatchable handles (VkSurfaceKHR, VkSwapchainKHR, VkSemaphore, ...)
struct Dev_T; struct Queue_T; struct Cmd_T;
typedef vk::Instance Inst; typedef vk::PhysicalDevice PDev; typedef Dev_T* Dev; typedef Queue_T* Queue; typedef Cmd_T* Cmd;
typedef void (*FnVoid)(void);

// ---- plain structs (Vulkan 1.0 layouts, 64-bit) ----
struct Ext2D { uint32_t width, height; };
struct Ext3D { uint32_t width, height, depth; };
struct Range { uint32_t aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount; };
struct QueueFamilyProperties { uint32_t queueFlags, queueCount, timestampValidBits; Ext3D minImageTransferGranularity; };
struct DeviceQueueCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, queueFamilyIndex, queueCount; const float* pQueuePriorities; };
struct DeviceCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, queueCreateInfoCount; const DeviceQueueCreateInfo* pQueueCreateInfos;
                          uint32_t enabledLayerCount; const char* const* ppEnabledLayerNames; uint32_t enabledExtensionCount; const char* const* ppEnabledExtensionNames; const void* pEnabledFeatures; };
struct Win32SurfaceCreateInfo { uint32_t sType; const void* pNext; uint32_t flags; void* hinstance; void* hwnd; };
struct SurfaceCapabilities { uint32_t minImageCount, maxImageCount; Ext2D currentExtent, minImageExtent, maxImageExtent;
                             uint32_t maxImageArrayLayers, supportedTransforms, currentTransform, supportedCompositeAlpha, supportedUsageFlags; };
struct SurfaceFormat { int32_t format, colorSpace; };
struct SwapchainCreateInfo { uint32_t sType; const void* pNext; uint32_t flags; H surface; uint32_t minImageCount; int32_t imageFormat, imageColorSpace;
                             Ext2D imageExtent; uint32_t imageArrayLayers, imageUsage, imageSharingMode, queueFamilyIndexCount; const uint32_t* pQueueFamilyIndices;
                             uint32_t preTransform, compositeAlpha, presentMode, clipped; H oldSwapchain; };
struct SemaphoreCreateInfo { uint32_t sType; const void* pNext; uint32_t flags; };
struct FenceCreateInfo { uint32_t sType; const void* pNext; uint32_t flags; };
struct CommandPoolCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, queueFamilyIndex; };
struct CommandBufferAllocateInfo { uint32_t sType; const void* pNext; H commandPool; uint32_t level, commandBufferCount; };
struct CommandBufferBeginInfo { uint32_t sType; const void* pNext; uint32_t flags; const void* pInheritanceInfo; };
struct ImageMemoryBarrier { uint32_t sType; const void* pNext; uint32_t srcAccessMask, dstAccessMask, oldLayout, newLayout, srcQueueFamilyIndex, dstQueueFamilyIndex;
                            H image; Range subresourceRange; };
struct SubmitInfo { uint32_t sType; const void* pNext; uint32_t waitSemaphoreCount; const H* pWaitSemaphores; const uint32_t* pWaitDstStageMask;
                    uint32_t commandBufferCount; const Cmd* pCommandBuffers; uint32_t signalSemaphoreCount; const H* pSignalSemaphores; };
struct PresentInfo { uint32_t sType; const void* pNext; uint32_t waitSemaphoreCount; const H* pWaitSemaphores; uint32_t swapchainCount; const H* pSwapchains;
                     const uint32_t* pImageIndices; R* pResults; };

// ---- structure types, enum values and constants (Vulkan 1.0 / VK_KHR_* values) ----
enum : uint32_t {
    S_WIN32_SURFACE = 1000009000u, S_SWAPCHAIN = 1000001000u, S_PRESENT = 1000001001u,
    S_SEMAPHORE = 9, S_FENCE = 8, S_COMMAND_POOL = 39, S_COMMAND_BUFFER_ALLOC = 40, S_COMMAND_BUFFER_BEGIN = 42, S_IMAGE_BARRIER = 45,
    S_SUBMIT = 4, S_DEVICE_QUEUE = 2, S_DEVICE = 3,
    QF_GRAPHICS = 0x1,
    STAGE_TOP = 0x1, STAGE_TRANSFER = 0x1000, STAGE_BOTTOM = 0x2000,
    ACCESS_TRANSFER_WRITE = 0x1000,
    LAYOUT_UNDEFINED = 0, LAYOUT_TRANSFER_DST = 7, LAYOUT_PRESENT = 1000001002u,
    FORMAT_BGRA8_UNORM = 44, USAGE_TRANSFER_DST = 0x2,
    PRESENT_IMMEDIATE = 0, PRESENT_MAILBOX = 1, PRESENT_FIFO = 2,
    ALPHA_OPAQUE = 0x1, ASPECT_COLOR = 0x1, POOL_RESET = 0x2, FENCE_SIGNALED = 0x1,
};
constexpr int32_t RES_OUT_OF_DATE = -1000001004;
constexpr int32_t RES_SUBOPTIMAL = 1000001003;
constexpr int kFramesInFlight = 2;       // CPU may run this many frames ahead of the GPU
constexpr int kMaxWindows = 16;
constexpr int kMaxImages = 8;

// ---- function pointer types (Vulkan 1.0 signatures) ----
typedef R (*PFN_vkCreateInstance)(const vk::InstanceCreateInfo*, const void*, Inst*);
typedef void (*PFN_vkDestroyInstance)(Inst, const void*);
typedef R (*PFN_vkEnumeratePhysicalDevices)(Inst, uint32_t*, PDev*);
typedef void (*PFN_vkGetPhysicalDeviceQueueFamilyProperties)(PDev, uint32_t*, QueueFamilyProperties*);
typedef void (*PFN_vkGetPhysicalDeviceProperties)(PDev, vk::PhysicalDeviceProperties*);
typedef R (*PFN_vkCreateDevice)(PDev, const DeviceCreateInfo*, const void*, Dev*);
typedef FnVoid (*PFN_vkGetDeviceProcAddr)(Dev, const char*);
typedef void (*PFN_vkDestroyDevice)(Dev, const void*);
typedef void (*PFN_vkGetDeviceQueue)(Dev, uint32_t, uint32_t, Queue*);
typedef R (*PFN_vkCreateWin32SurfaceKHR)(Inst, const Win32SurfaceCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroySurfaceKHR)(Inst, H, const void*);
typedef R (*PFN_vkGetPhysicalDeviceSurfaceSupportKHR)(PDev, uint32_t, H, uint32_t*);
typedef R (*PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)(PDev, H, SurfaceCapabilities*);
typedef R (*PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)(PDev, H, uint32_t*, SurfaceFormat*);
typedef R (*PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)(PDev, H, uint32_t*, uint32_t*);
typedef R (*PFN_vkCreateSwapchainKHR)(Dev, const SwapchainCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroySwapchainKHR)(Dev, H, const void*);
typedef R (*PFN_vkGetSwapchainImagesKHR)(Dev, H, uint32_t*, H*);
typedef R (*PFN_vkAcquireNextImageKHR)(Dev, H, uint64_t, H, H, uint32_t*);
typedef R (*PFN_vkQueuePresentKHR)(Queue, const PresentInfo*);
typedef R (*PFN_vkCreateSemaphore)(Dev, const SemaphoreCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroySemaphore)(Dev, H, const void*);
typedef R (*PFN_vkCreateFence)(Dev, const FenceCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyFence)(Dev, H, const void*);
typedef R (*PFN_vkWaitForFences)(Dev, uint32_t, const H*, uint32_t, uint64_t);
typedef R (*PFN_vkResetFences)(Dev, uint32_t, const H*);
typedef R (*PFN_vkCreateCommandPool)(Dev, const CommandPoolCreateInfo*, const void*, H*);
typedef R (*PFN_vkAllocateCommandBuffers)(Dev, const CommandBufferAllocateInfo*, Cmd*);
typedef R (*PFN_vkBeginCommandBuffer)(Cmd, const CommandBufferBeginInfo*);
typedef R (*PFN_vkEndCommandBuffer)(Cmd);
typedef void (*PFN_vkCmdPipelineBarrier)(Cmd, uint32_t, uint32_t, uint32_t, uint32_t, const void*, uint32_t, const void*, uint32_t, const ImageMemoryBarrier*);
typedef void (*PFN_vkCmdClearColorImage)(Cmd, H, uint32_t, const float*, uint32_t, const Range*);
typedef R (*PFN_vkQueueSubmit)(Queue, uint32_t, const SubmitInfo*, H);
typedef R (*PFN_vkDeviceWaitIdle)(Dev);
typedef void (*PFN_vkDestroyCommandPool)(Dev, H, const void*);

// every device/instance entry point the presentation path calls, loaded once at startup
struct Ptrs {
    PFN_vkGetPhysicalDeviceQueueFamilyProperties qfProps = nullptr;
    PFN_vkGetPhysicalDeviceProperties props = nullptr;
    PFN_vkCreateDevice createDevice = nullptr;
    PFN_vkDestroyDevice destroyDevice = nullptr;
    PFN_vkGetDeviceQueue getQueue = nullptr;
    PFN_vkCreateWin32SurfaceKHR createSurface = nullptr;
    PFN_vkDestroySurfaceKHR destroySurface = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR surfaceSupport = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR surfaceCaps = nullptr;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR surfaceFormats = nullptr;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR presentModes = nullptr;
    PFN_vkCreateSwapchainKHR createSwapchain = nullptr;
    PFN_vkDestroySwapchainKHR destroySwapchain = nullptr;
    PFN_vkGetSwapchainImagesKHR swapImages = nullptr;
    PFN_vkAcquireNextImageKHR acquire = nullptr;
    PFN_vkQueuePresentKHR present = nullptr;
    PFN_vkCreateSemaphore createSemaphore = nullptr;
    PFN_vkDestroySemaphore destroySemaphore = nullptr;
    PFN_vkCreateFence createFence = nullptr;
    PFN_vkDestroyFence destroyFence = nullptr;
    PFN_vkWaitForFences waitFences = nullptr;
    PFN_vkResetFences resetFences = nullptr;
    PFN_vkCreateCommandPool createPool = nullptr;
    PFN_vkAllocateCommandBuffers allocCmd = nullptr;
    PFN_vkBeginCommandBuffer beginCmd = nullptr;
    PFN_vkEndCommandBuffer endCmd = nullptr;
    PFN_vkCmdPipelineBarrier barrier = nullptr;
    PFN_vkCmdClearColorImage clear = nullptr;
    PFN_vkQueueSubmit submit = nullptr;
    PFN_vkDeviceWaitIdle deviceWaitIdle = nullptr;
    PFN_vkDestroyCommandPool destroyPool = nullptr;
    PFN_vkDestroyInstance destroyInstance = nullptr;
};

// the one Vulkan context shared by every window
struct Ctx {
    bool ready = false;
    vk::PFN_GetInstanceProcAddr gipa = nullptr;
    PFN_vkGetDeviceProcAddr getDevProc = nullptr;
    Inst inst = nullptr;
    PDev pd = nullptr;
    Dev dev = nullptr;
    Queue q = nullptr;
    uint32_t qf = 0;
    H pool = 0;
    Cmd cmd[kFramesInFlight] = {};
    uint32_t presentMode = PRESENT_FIFO;
    uint32_t modeCount = 0, modes[8] = {};
    Ptrs f;
    char gpu[256] = {};
};

// per window: a swapchain plus the synchronization that lets frames overlap
struct WinSwap {
    void* hwnd = nullptr;
    H surface = 0, swap = 0;
    H fence[kFramesInFlight] = {};
    H semAcquire[kFramesInFlight] = {};
    H semRender[kMaxImages] = {};        // one per swapchain image: presentation waits on it
    uint32_t w = 0, h = 0, format = 0, colorSpace = 0, alpha = 0, n = 0;
    H imgs[kMaxImages] = {};
    uint32_t frame = 0;                  // which in-flight slot is next
    bool sync_ready = false;
};

extern Ctx g_ctx;
extern WinSwap g_wins[kMaxWindows];
extern int g_nwins;

// ---- steps of context creation and per-window setup (each lives in its own file) ----
bool vk_create_instance(const char** err);            // vk_instance.cpp
bool vk_pick_device(const char** err);                // vk_instance.cpp
bool vk_create_device(const char** err);              // vk_device.cpp
bool vk_init_context(const char** err);               // vk_device.cpp: runs the steps above once
bool vk_make_sync(WinSwap& s, const char** err);      // vk_sync.cpp
bool vk_make_swapchain(WinSwap& s, uint32_t w, uint32_t h, const char** err);   // vk_swapchain.cpp
void vk_choose_present_mode(H surface);               // vk_swapchain.cpp
WinSwap* vk_window_state(void* hinst, void* hwnd, uint32_t w, uint32_t h, const char** err);   // vk_window.cpp
void vk_release_window(void* hwnd);                    // vk_release.cpp: frees one window's GPU objects; the last one also shuts the context down
void vk_shutdown();                                    // vk_release.cpp: destroys pool, device and instance; safe to call when not initialised

}}  // namespace evgx::vkp
