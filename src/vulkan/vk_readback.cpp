// Vulkan backend, step 1: offscreen clear + readback on the GPU. Declarations are hand-written (no SDK).
#include <stdint.h>
#include <stddef.h>
#include <evg/vk_min.h>
#include "platform.hpp"

namespace evgx { namespace vkb {

typedef int32_t R;                    // VkResult
typedef uint64_t H;                   // non-dispatchable handle (VkImage, VkBuffer, VkDeviceMemory, ...)
struct Dev_T; struct Queue_T; struct Cmd_T;
typedef vk::Instance Inst; typedef vk::PhysicalDevice PDev; typedef Dev_T* Dev; typedef Queue_T* Queue; typedef Cmd_T* Cmd;
typedef void (*FnVoid)(void);

struct Extent3D { uint32_t width, height, depth; };
struct Offset3D { int32_t x, y, z; };
struct SubresourceRange { uint32_t aspectMask, baseMipLevel, levelCount, baseArrayLayer, layerCount; };
struct SubresourceLayers { uint32_t aspectMask, mipLevel, baseArrayLayer, layerCount; };

struct QueueFamilyProperties { uint32_t queueFlags, queueCount, timestampValidBits; Extent3D minImageTransferGranularity; };
struct DeviceQueueCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, queueFamilyIndex, queueCount; const float* pQueuePriorities; };
struct DeviceCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, queueCreateInfoCount; const DeviceQueueCreateInfo* pQueueCreateInfos;
                          uint32_t enabledLayerCount; const char* const* ppEnabledLayerNames; uint32_t enabledExtensionCount; const char* const* ppEnabledExtensionNames; const void* pEnabledFeatures; };
struct CommandPoolCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, queueFamilyIndex; };
struct CommandBufferAllocateInfo { uint32_t sType; const void* pNext; H commandPool; uint32_t level, commandBufferCount; };
struct CommandBufferBeginInfo { uint32_t sType; const void* pNext; uint32_t flags; const void* pInheritanceInfo; };
struct ImageCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, imageType, format; Extent3D extent; uint32_t mipLevels, arrayLayers, samples, tiling, usage, sharingMode,
                         queueFamilyIndexCount; const uint32_t* pQueueFamilyIndices; uint32_t initialLayout; };
struct BufferCreateInfo { uint32_t sType; const void* pNext; uint32_t flags; uint64_t size; uint32_t usage, sharingMode, queueFamilyIndexCount; const uint32_t* pQueueFamilyIndices; };
struct MemoryRequirements { uint64_t size, alignment; uint32_t memoryTypeBits; };
struct MemoryAllocateInfo { uint32_t sType; const void* pNext; uint64_t allocationSize; uint32_t memoryTypeIndex; };
struct MemoryType { uint32_t propertyFlags, heapIndex; };
struct MemoryHeap { uint64_t size; uint32_t flags; };
struct PhysicalDeviceMemoryProperties { uint32_t memoryTypeCount; MemoryType memoryTypes[32]; uint32_t memoryHeapCount; MemoryHeap memoryHeaps[16]; };
struct ImageMemoryBarrier { uint32_t sType; const void* pNext; uint32_t srcAccessMask, dstAccessMask, oldLayout, newLayout, srcQueueFamilyIndex, dstQueueFamilyIndex;
                            H image; SubresourceRange subresourceRange; };
struct BufferImageCopy { uint64_t bufferOffset; uint32_t bufferRowLength, bufferImageHeight; SubresourceLayers imageSubresource; Offset3D imageOffset; Extent3D imageExtent; };
struct SubmitInfo { uint32_t sType; const void* pNext; uint32_t waitSemaphoreCount; const H* pWaitSemaphores; const uint32_t* pWaitDstStageMask;
                    uint32_t commandBufferCount; const Cmd* pCommandBuffers; uint32_t signalSemaphoreCount; const H* pSignalSemaphores; };

// function pointer types (Vulkan 1.0 signatures)
typedef R (*PFN_vkCreateInstance)(const vk::InstanceCreateInfo*, const void*, Inst*);
typedef void (*PFN_vkDestroyInstance)(Inst, const void*);
typedef R (*PFN_vkEnumeratePhysicalDevices)(Inst, uint32_t*, PDev*);
typedef void (*PFN_vkGetPhysicalDeviceQueueFamilyProperties)(PDev, uint32_t*, QueueFamilyProperties*);
typedef void (*PFN_vkGetPhysicalDeviceMemoryProperties)(PDev, PhysicalDeviceMemoryProperties*);
typedef R (*PFN_vkCreateDevice)(PDev, const DeviceCreateInfo*, const void*, Dev*);
typedef FnVoid (*PFN_vkGetDeviceProcAddr)(Dev, const char*);
typedef void (*PFN_vkDestroyDevice)(Dev, const void*);
typedef void (*PFN_vkGetDeviceQueue)(Dev, uint32_t, uint32_t, Queue*);
typedef R (*PFN_vkCreateCommandPool)(Dev, const CommandPoolCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyCommandPool)(Dev, H, const void*);
typedef R (*PFN_vkAllocateCommandBuffers)(Dev, const CommandBufferAllocateInfo*, Cmd*);
typedef R (*PFN_vkBeginCommandBuffer)(Cmd, const CommandBufferBeginInfo*);
typedef R (*PFN_vkEndCommandBuffer)(Cmd);
typedef void (*PFN_vkCmdPipelineBarrier)(Cmd, uint32_t, uint32_t, uint32_t, uint32_t, const void*, uint32_t, const void*, uint32_t, const ImageMemoryBarrier*);
typedef void (*PFN_vkCmdClearColorImage)(Cmd, H, uint32_t, const float*, uint32_t, const SubresourceRange*);
typedef void (*PFN_vkCmdCopyImageToBuffer)(Cmd, H, uint32_t, H, uint32_t, const BufferImageCopy*);
typedef R (*PFN_vkQueueSubmit)(Queue, uint32_t, const SubmitInfo*, H);
typedef R (*PFN_vkQueueWaitIdle)(Queue);
typedef R (*PFN_vkDeviceWaitIdle)(Dev);
typedef R (*PFN_vkCreateImage)(Dev, const ImageCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyImage)(Dev, H, const void*);
typedef void (*PFN_vkGetImageMemoryRequirements)(Dev, H, MemoryRequirements*);
typedef R (*PFN_vkBindImageMemory)(Dev, H, H, uint64_t);
typedef R (*PFN_vkCreateBuffer)(Dev, const BufferCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyBuffer)(Dev, H, const void*);
typedef void (*PFN_vkGetBufferMemoryRequirements)(Dev, H, MemoryRequirements*);
typedef R (*PFN_vkBindBufferMemory)(Dev, H, H, uint64_t);
typedef R (*PFN_vkAllocateMemory)(Dev, const MemoryAllocateInfo*, const void*, H*);
typedef void (*PFN_vkFreeMemory)(Dev, H, const void*);
typedef R (*PFN_vkMapMemory)(Dev, H, uint64_t, uint64_t, uint32_t, void**);
typedef void (*PFN_vkUnmapMemory)(Dev, H);
typedef void (*PFN_vkGetPhysicalDeviceProperties)(PDev, vk::PhysicalDeviceProperties*);

// pipeline / image / access constants
enum : uint32_t {
    QF_GRAPHICS = 0x1, QF_TRANSFER = 0x4,
    STAGE_TOP_OF_PIPE = 0x1, STAGE_TRANSFER = 0x1000,
    ACCESS_TRANSFER_READ = 0x800, ACCESS_TRANSFER_WRITE = 0x1000,
    LAYOUT_UNDEFINED = 0, LAYOUT_TRANSFER_SRC = 6, LAYOUT_TRANSFER_DST = 7,
    FORMAT_B8G8R8A8_UNORM = 44,
    IMAGE_USAGE_TRANSFER_SRC = 0x1, IMAGE_USAGE_TRANSFER_DST = 0x2,
    BUFFER_USAGE_TRANSFER_DST = 0x2,
    MEM_DEVICE_LOCAL = 0x1, MEM_HOST_VISIBLE = 0x2, MEM_HOST_COHERENT = 0x4,
    ASPECT_COLOR = 0x1,
    STRUCT_INSTANCE_CREATE = 1, STRUCT_DEVICE_QUEUE_CREATE = 2, STRUCT_DEVICE_CREATE = 3, STRUCT_SUBMIT = 4,
    STRUCT_MEMORY_ALLOCATE = 5, STRUCT_FENCE = 8, STRUCT_SEMAPHORE = 9, STRUCT_BUFFER_CREATE = 12, STRUCT_IMAGE_CREATE = 14,
    STRUCT_COMMAND_POOL_CREATE = 39, STRUCT_COMMAND_BUFFER_ALLOCATE = 40, STRUCT_COMMAND_BUFFER_BEGIN = 42, STRUCT_IMAGE_MEMORY_BARRIER = 45,
};

namespace {
struct Fns {
    PFN_vkGetPhysicalDeviceProperties getPhysicalDeviceProperties;
    PFN_vkGetPhysicalDeviceQueueFamilyProperties getQueueFamilies;
    PFN_vkGetPhysicalDeviceMemoryProperties getMemoryProperties;
    PFN_vkCreateDevice createDevice;
    PFN_vkDestroyDevice destroyDevice;
    PFN_vkGetDeviceQueue getDeviceQueue;
    PFN_vkCreateCommandPool createCommandPool;
    PFN_vkDestroyCommandPool destroyCommandPool;
    PFN_vkAllocateCommandBuffers allocCommandBuffers;
    PFN_vkBeginCommandBuffer beginCommandBuffer;
    PFN_vkEndCommandBuffer endCommandBuffer;
    PFN_vkCmdPipelineBarrier cmdPipelineBarrier;
    PFN_vkCmdClearColorImage cmdClearColorImage;
    PFN_vkCmdCopyImageToBuffer cmdCopyImageToBuffer;
    PFN_vkQueueSubmit queueSubmit;
    PFN_vkQueueWaitIdle queueWaitIdle;
    PFN_vkDeviceWaitIdle deviceWaitIdle;
    PFN_vkCreateImage createImage;
    PFN_vkDestroyImage destroyImage;
    PFN_vkGetImageMemoryRequirements imageMemReq;
    PFN_vkBindImageMemory bindImageMemory;
    PFN_vkCreateBuffer createBuffer;
    PFN_vkDestroyBuffer destroyBuffer;
    PFN_vkGetBufferMemoryRequirements bufferMemReq;
    PFN_vkBindBufferMemory bindBufferMemory;
    PFN_vkAllocateMemory allocMemory;
    PFN_vkFreeMemory freeMemory;
    PFN_vkMapMemory mapMemory;
    PFN_vkUnmapMemory unmapMemory;
};

// finds a memory type with all `want` bits set (and none of `not_want` required)
int find_memtype(const PhysicalDeviceMemoryProperties& mp, uint32_t typeBits, uint32_t want) {
    for (uint32_t i = 0; i < mp.memoryTypeCount; i++)
        if ((typeBits & (1u << i)) && (mp.memoryTypes[i].propertyFlags & want) == want) return (int)i;
    return -1;
}
}

// Clear an image to (r,g,b) on the GPU, copy it back, write ARGB pixels into `out` (w*h uint32).
bool clear_readback(uint32_t width, uint32_t height, float r, float g, float b, uint32_t* out, const char** err) {
    *err = nullptr;
    if (!width || !height || !out) { *err = "bad size"; return false; }
    auto gipa = (vk::PFN_GetInstanceProcAddr)plat::load_vulkan();
    if (!gipa) { *err = "Vulkan runtime not found"; return false; }

    auto create = (PFN_vkCreateInstance)gipa(nullptr, "vkCreateInstance");
    vk::ApplicationInfo app = {};
    app.sType = vk::STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "evg"; app.pEngineName = "evg"; app.apiVersion = vk::make_version(0, 1, 0, 0);
    vk::InstanceCreateInfo ici = {};
    ici.sType = vk::STRUCTURE_TYPE_INSTANCE_CREATE_INFO; ici.pApplicationInfo = &app;
    Inst inst = nullptr;
    if (!create || create(&ici, nullptr, &inst) != 0 || !inst) { *err = "vkCreateInstance failed"; return false; }

    auto enumPD = (PFN_vkEnumeratePhysicalDevices)gipa(inst, "vkEnumeratePhysicalDevices");
    Fns f = {};
    f.getPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)gipa(inst, "vkGetPhysicalDeviceProperties");
    f.getQueueFamilies = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)gipa(inst, "vkGetPhysicalDeviceQueueFamilyProperties");
    f.getMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)gipa(inst, "vkGetPhysicalDeviceMemoryProperties");
    f.createDevice = (PFN_vkCreateDevice)gipa(inst, "vkCreateDevice");
    auto getDevProc = (PFN_vkGetDeviceProcAddr)gipa(inst, "vkGetDeviceProcAddr");
    auto destroyInst = (PFN_vkDestroyInstance)gipa(inst, "vkDestroyInstance");
    if (!enumPD || !f.getQueueFamilies || !f.getMemoryProperties || !f.createDevice || !getDevProc) { *err = "instance functions missing"; return false; }

    // pick the physical device: discrete > integrated > other, must have a graphics queue
    PDev pds[8]; uint32_t npd = 8;
    enumPD(inst, &npd, pds);
    PDev pd = nullptr; uint32_t qf = 0; int best = -1;
    for (uint32_t i = 0; i < npd; i++) {
        uint32_t nq = 0; f.getQueueFamilies(pds[i], &nq, nullptr);
        QueueFamilyProperties qfp[16]; if (nq > 16) nq = 16;
        f.getQueueFamilies(pds[i], &nq, qfp);
        int fam = -1;
        for (uint32_t k = 0; k < nq; k++) if (qfp[k].queueFlags & QF_GRAPHICS) { fam = (int)k; break; }
        if (fam < 0) continue;
        vk::PhysicalDeviceProperties p; f.getPhysicalDeviceProperties(pds[i], &p);
        int score = p.deviceType == vk::DEVICE_TYPE_DISCRETE ? 3 : p.deviceType == vk::DEVICE_TYPE_INTEGRATED ? 2 : 1;
        if (score > best) { best = score; pd = pds[i]; qf = (uint32_t)fam; }
    }
    if (!pd) { *err = "no GPU with a graphics queue"; destroyInst(inst, nullptr); return false; }

    // device + queue
    float prio = 1.0f;
    DeviceQueueCreateInfo qci = {}; qci.sType = STRUCT_DEVICE_QUEUE_CREATE; qci.queueFamilyIndex = qf; qci.queueCount = 1; qci.pQueuePriorities = &prio;
    DeviceCreateInfo dci = {}; dci.sType = STRUCT_DEVICE_CREATE; dci.queueCreateInfoCount = 1; dci.pQueueCreateInfos = &qci;
    Dev dev = nullptr;
    if (f.createDevice(pd, &dci, nullptr, &dev) != 0 || !dev) { *err = "vkCreateDevice failed"; destroyInst(inst, nullptr); return false; }
    f.destroyDevice = (PFN_vkDestroyDevice)getDevProc(dev, "vkDestroyDevice");
    f.getDeviceQueue = (PFN_vkGetDeviceQueue)getDevProc(dev, "vkGetDeviceQueue");
    f.createCommandPool = (PFN_vkCreateCommandPool)getDevProc(dev, "vkCreateCommandPool");
    f.destroyCommandPool = (PFN_vkDestroyCommandPool)getDevProc(dev, "vkDestroyCommandPool");
    f.allocCommandBuffers = (PFN_vkAllocateCommandBuffers)getDevProc(dev, "vkAllocateCommandBuffers");
    f.beginCommandBuffer = (PFN_vkBeginCommandBuffer)getDevProc(dev, "vkBeginCommandBuffer");
    f.endCommandBuffer = (PFN_vkEndCommandBuffer)getDevProc(dev, "vkEndCommandBuffer");
    f.cmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)getDevProc(dev, "vkCmdPipelineBarrier");
    f.cmdClearColorImage = (PFN_vkCmdClearColorImage)getDevProc(dev, "vkCmdClearColorImage");
    f.cmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)getDevProc(dev, "vkCmdCopyImageToBuffer");
    f.queueSubmit = (PFN_vkQueueSubmit)getDevProc(dev, "vkQueueSubmit");
    f.queueWaitIdle = (PFN_vkQueueWaitIdle)getDevProc(dev, "vkQueueWaitIdle");
    f.deviceWaitIdle = (PFN_vkDeviceWaitIdle)getDevProc(dev, "vkDeviceWaitIdle");
    f.createImage = (PFN_vkCreateImage)getDevProc(dev, "vkCreateImage");
    f.destroyImage = (PFN_vkDestroyImage)getDevProc(dev, "vkDestroyImage");
    f.imageMemReq = (PFN_vkGetImageMemoryRequirements)getDevProc(dev, "vkGetImageMemoryRequirements");
    f.bindImageMemory = (PFN_vkBindImageMemory)getDevProc(dev, "vkBindImageMemory");
    f.createBuffer = (PFN_vkCreateBuffer)getDevProc(dev, "vkCreateBuffer");
    f.destroyBuffer = (PFN_vkDestroyBuffer)getDevProc(dev, "vkDestroyBuffer");
    f.bufferMemReq = (PFN_vkGetBufferMemoryRequirements)getDevProc(dev, "vkGetBufferMemoryRequirements");
    f.bindBufferMemory = (PFN_vkBindBufferMemory)getDevProc(dev, "vkBindBufferMemory");
    f.allocMemory = (PFN_vkAllocateMemory)getDevProc(dev, "vkAllocateMemory");
    f.freeMemory = (PFN_vkFreeMemory)getDevProc(dev, "vkFreeMemory");
    f.mapMemory = (PFN_vkMapMemory)getDevProc(dev, "vkMapMemory");
    f.unmapMemory = (PFN_vkUnmapMemory)getDevProc(dev, "vkUnmapMemory");

    Queue queue = nullptr;
    f.getDeviceQueue(dev, qf, 0, &queue);

    PhysicalDeviceMemoryProperties mp = {};
    f.getMemoryProperties(pd, &mp);

    // command pool + buffer
    CommandPoolCreateInfo pci = {}; pci.sType = STRUCT_COMMAND_POOL_CREATE; pci.queueFamilyIndex = qf;
    H pool = 0;
    f.createCommandPool(dev, &pci, nullptr, &pool);
    CommandBufferAllocateInfo cbai = {}; cbai.sType = STRUCT_COMMAND_BUFFER_ALLOCATE; cbai.commandPool = pool; cbai.level = 0; cbai.commandBufferCount = 1;
    Cmd cmd = nullptr;
    f.allocCommandBuffers(dev, &cbai, &cmd);

    // image (the render target) and host-visible buffer (readback)
    ImageCreateInfo ici2 = {};
    ici2.sType = STRUCT_IMAGE_CREATE; ici2.imageType = 1; ici2.format = FORMAT_B8G8R8A8_UNORM;
    ici2.extent = Extent3D{width, height, 1}; ici2.mipLevels = 1; ici2.arrayLayers = 1; ici2.samples = 1;
    ici2.tiling = 0; ici2.usage = IMAGE_USAGE_TRANSFER_SRC | IMAGE_USAGE_TRANSFER_DST; ici2.sharingMode = 0; ici2.initialLayout = LAYOUT_UNDEFINED;
    H image = 0;
    if (f.createImage(dev, &ici2, nullptr, &image) != 0) { *err = "vkCreateImage failed"; return false; }
    MemoryRequirements imr = {};
    f.imageMemReq(dev, image, &imr);
    int imt = find_memtype(mp, imr.memoryTypeBits, MEM_DEVICE_LOCAL);
    if (imt < 0) { *err = "no device-local memory type"; return false; }
    MemoryAllocateInfo mai = {}; mai.sType = STRUCT_MEMORY_ALLOCATE; mai.allocationSize = imr.size; mai.memoryTypeIndex = (uint32_t)imt;
    H imageMem = 0;
    f.allocMemory(dev, &mai, nullptr, &imageMem);
    f.bindImageMemory(dev, image, imageMem, 0);

    uint64_t bytes = (uint64_t)width * height * 4;
    BufferCreateInfo bci = {}; bci.sType = STRUCT_BUFFER_CREATE; bci.size = bytes; bci.usage = BUFFER_USAGE_TRANSFER_DST; bci.sharingMode = 0;
    H buffer = 0;
    f.createBuffer(dev, &bci, nullptr, &buffer);
    MemoryRequirements bmr = {};
    f.bufferMemReq(dev, buffer, &bmr);
    int bmt = find_memtype(mp, bmr.memoryTypeBits, MEM_HOST_VISIBLE | MEM_HOST_COHERENT);
    if (bmt < 0) { *err = "no host-visible memory type"; return false; }
    MemoryAllocateInfo bai = {}; bai.sType = STRUCT_MEMORY_ALLOCATE; bai.allocationSize = bmr.size; bai.memoryTypeIndex = (uint32_t)bmt;
    H bufMem = 0;
    f.allocMemory(dev, &bai, nullptr, &bufMem);
    f.bindBufferMemory(dev, buffer, bufMem, 0);

    // record: UNDEFINED -> TRANSFER_DST, clear, TRANSFER_DST -> TRANSFER_SRC, copy to buffer
    CommandBufferBeginInfo cbbi = {}; cbbi.sType = STRUCT_COMMAND_BUFFER_BEGIN; cbbi.flags = 1;   // ONE_TIME_SUBMIT
    f.beginCommandBuffer(cmd, &cbbi);
    SubresourceRange full = {ASPECT_COLOR, 0, 1, 0, 1};
    ImageMemoryBarrier b1 = {};
    b1.sType = STRUCT_IMAGE_MEMORY_BARRIER; b1.srcAccessMask = 0; b1.dstAccessMask = ACCESS_TRANSFER_WRITE;
    b1.oldLayout = LAYOUT_UNDEFINED; b1.newLayout = LAYOUT_TRANSFER_DST; b1.srcQueueFamilyIndex = 0xFFFFFFFFu; b1.dstQueueFamilyIndex = 0xFFFFFFFFu;
    b1.image = image; b1.subresourceRange = full;
    f.cmdPipelineBarrier(cmd, STAGE_TOP_OF_PIPE, STAGE_TRANSFER, 0, 0, nullptr, 0, nullptr, 1, &b1);
    float color[4] = {r, g, b, 1.0f};
    f.cmdClearColorImage(cmd, image, LAYOUT_TRANSFER_DST, color, 1, &full);
    ImageMemoryBarrier b2 = b1;
    b2.srcAccessMask = ACCESS_TRANSFER_WRITE; b2.dstAccessMask = ACCESS_TRANSFER_READ;
    b2.oldLayout = LAYOUT_TRANSFER_DST; b2.newLayout = LAYOUT_TRANSFER_SRC;
    f.cmdPipelineBarrier(cmd, STAGE_TRANSFER, STAGE_TRANSFER, 0, 0, nullptr, 0, nullptr, 1, &b2);
    BufferImageCopy region = {};
    region.imageSubresource = SubresourceLayers{ASPECT_COLOR, 0, 0, 1};
    region.imageExtent = Extent3D{width, height, 1};
    f.cmdCopyImageToBuffer(cmd, image, LAYOUT_TRANSFER_SRC, buffer, 1, &region);
    f.endCommandBuffer(cmd);

    SubmitInfo si = {}; si.sType = STRUCT_SUBMIT; si.commandBufferCount = 1; si.pCommandBuffers = &cmd;
    f.queueSubmit(queue, 1, &si, 0);
    f.queueWaitIdle(queue);

    void* mapped = nullptr;
    if (f.mapMemory(dev, bufMem, 0, bytes, 0, &mapped) != 0 || !mapped) { *err = "vkMapMemory failed"; return false; }
    const uint32_t* px = (const uint32_t*)mapped;
    for (uint64_t i = 0; i < (uint64_t)width * height; i++) out[i] = px[i];
    f.unmapMemory(dev, bufMem);

    // teardown
    f.destroyBuffer(dev, buffer, nullptr);
    f.freeMemory(dev, bufMem, nullptr);
    f.destroyImage(dev, image, nullptr);
    f.freeMemory(dev, imageMem, nullptr);
    f.destroyCommandPool(dev, pool, nullptr);
    f.deviceWaitIdle(dev);
    f.destroyDevice(dev, nullptr);
    destroyInst(inst, nullptr);
    return true;
}

}}  // namespace evgx::vkb
