// Minimal hand-written Vulkan declarations. No Vulkan SDK/headers needed; the driver is loaded at runtime.
#pragma once
#include <stdint.h>
static_assert(sizeof(void*) == 8, "evg targets 64-bit only");

namespace evgx { namespace vk {
typedef int32_t Result;
enum : Result { SUCCESS = 0 };
typedef struct Instance_T* Instance;
typedef struct PhysicalDevice_T* PhysicalDevice;
typedef void (*VoidFn)(void);
typedef VoidFn (*PFN_GetInstanceProcAddr)(Instance, const char*);

struct ApplicationInfo {
    uint32_t sType; const void* pNext; const char* pApplicationName; uint32_t applicationVersion;
    const char* pEngineName; uint32_t engineVersion; uint32_t apiVersion;
};
struct InstanceCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags; const ApplicationInfo* pApplicationInfo;
    uint32_t enabledLayerCount; const char* const* ppEnabledLayerNames;
    uint32_t enabledExtensionCount; const char* const* ppEnabledExtensionNames;
};
// Layout-compatible with VkPhysicalDeviceProperties (824 bytes on 64-bit); limits/sparse kept opaque.
struct PhysicalDeviceProperties {
    uint32_t apiVersion, driverVersion, vendorID, deviceID, deviceType;
    char deviceName[256];
    uint8_t pipelineCacheUUID[16];
    alignas(8) uint8_t limitsAndSparse[824 - 296];
};
static_assert(sizeof(PhysicalDeviceProperties) == 824, "VkPhysicalDeviceProperties layout mismatch");

enum : uint32_t { STRUCTURE_TYPE_APPLICATION_INFO = 0, STRUCTURE_TYPE_INSTANCE_CREATE_INFO = 1 };
enum : uint32_t { DEVICE_TYPE_OTHER = 0, DEVICE_TYPE_INTEGRATED = 1, DEVICE_TYPE_DISCRETE = 2, DEVICE_TYPE_VIRTUAL = 3, DEVICE_TYPE_CPU = 4 };
constexpr uint32_t make_version(uint32_t v, uint32_t maj, uint32_t min, uint32_t patch) { return (v << 29) | (maj << 22) | (min << 12) | patch; }

typedef Result (*PFN_CreateInstance)(const InstanceCreateInfo*, const void*, Instance*);
typedef void (*PFN_DestroyInstance)(Instance, const void*);
typedef Result (*PFN_EnumeratePhysicalDevices)(Instance, uint32_t*, PhysicalDevice*);
typedef void (*PFN_GetPhysicalDeviceProperties)(PhysicalDevice, PhysicalDeviceProperties*);
}} // namespace evgx::vk
