// Vulkan probe: loads the driver, creates an instance and picks the best GPU (name only; drawing is in src/vulkan).
#include "state.hpp"
#include "platform.hpp"

namespace evgx {
using namespace st;

bool Api::init() {
    if (g_inited) return g_init_ok;
    g_inited = true;
    g_gpu[0] = 0;

    g_gipa = (vk::PFN_GetInstanceProcAddr)plat::load_vulkan();
    if (!g_gipa) return fail("Vulkan runtime not found (no vulkan-1.dll / libvulkan.so)");
    auto create = (vk::PFN_CreateInstance)g_gipa(nullptr, "vkCreateInstance");
    if (!create) return fail("vkCreateInstance missing");

    vk::ApplicationInfo app = {};
    app.sType = vk::STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "evg app"; app.pEngineName = "evg";
    app.apiVersion = vk::make_version(0, 1, 0, 0);
    vk::InstanceCreateInfo ci = {};
    ci.sType = vk::STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ci.pApplicationInfo = &app;
    if (create(&ci, nullptr, &g_instance) != vk::SUCCESS || !g_instance) return fail("vkCreateInstance failed");

    auto enumerate = (vk::PFN_EnumeratePhysicalDevices)g_gipa(g_instance, "vkEnumeratePhysicalDevices");
    auto props_fn  = (vk::PFN_GetPhysicalDeviceProperties)g_gipa(g_instance, "vkGetPhysicalDeviceProperties");
    if (!enumerate || !props_fn) return fail("instance functions missing");

    vk::PhysicalDevice devs[8]; uint32_t n = 8;
    vk::Result r = enumerate(g_instance, &n, devs);
    if ((r != vk::SUCCESS && r != 5 /*INCOMPLETE*/) || n == 0) return fail("no Vulkan-capable GPU found");

    int best_score = -1;
    for (uint32_t i = 0; i < n; i++) {
        vk::PhysicalDeviceProperties p;
        props_fn(devs[i], &p);
        int score = p.deviceType == vk::DEVICE_TYPE_DISCRETE ? 3 : p.deviceType == vk::DEVICE_TYPE_INTEGRATED ? 2
                  : p.deviceType == vk::DEVICE_TYPE_CPU ? 0 : 1;
        if (score > best_score) { best_score = score; st::copy(g_gpu, p.deviceName, sizeof g_gpu); }
    }
    g_init_ok = true;
    return true;
}

}  // namespace evgx
