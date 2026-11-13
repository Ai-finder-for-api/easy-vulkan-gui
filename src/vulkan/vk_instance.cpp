// Vulkan instance creation (with surface extensions) and physical device selection.
#include "vk_types.hpp"

namespace evgx { namespace vkp {

namespace {
const char* kInstanceExt[] = {"VK_KHR_surface", "VK_KHR_win32_surface"};
}

bool vk_create_instance(const char** err) {
    g_ctx.gipa = (vk::PFN_GetInstanceProcAddr)plat::load_vulkan();
    if (!g_ctx.gipa) { *err = "Vulkan runtime not found"; return false; }

    vk::ApplicationInfo app = {};
    app.sType = vk::STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName = "evg";
    app.pEngineName = "evg";
    app.apiVersion = vk::make_version(0, 1, 0, 0);
    vk::InstanceCreateInfo ici = {};
    ici.sType = vk::STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &app;
    ici.enabledExtensionCount = 2;
    ici.ppEnabledExtensionNames = kInstanceExt;

    auto create = (PFN_vkCreateInstance)g_ctx.gipa(nullptr, "vkCreateInstance");
    if (!create || create(&ici, nullptr, &g_ctx.inst) != 0 || !g_ctx.inst) {
        *err = "vkCreateInstance failed (surface extensions)";
        return false;
    }

    // instance-level entry points used by the rest of the backend
    Inst inst = g_ctx.inst;
    g_ctx.f.qfProps = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)g_ctx.gipa(inst, "vkGetPhysicalDeviceQueueFamilyProperties");
    g_ctx.f.props = (PFN_vkGetPhysicalDeviceProperties)g_ctx.gipa(inst, "vkGetPhysicalDeviceProperties");
    g_ctx.f.createDevice = (PFN_vkCreateDevice)g_ctx.gipa(inst, "vkCreateDevice");
    g_ctx.f.destroyInstance = (PFN_vkDestroyInstance)g_ctx.gipa(inst, "vkDestroyInstance");
    g_ctx.f.createSurface = (PFN_vkCreateWin32SurfaceKHR)g_ctx.gipa(inst, "vkCreateWin32SurfaceKHR");
    g_ctx.f.destroySurface = (PFN_vkDestroySurfaceKHR)g_ctx.gipa(inst, "vkDestroySurfaceKHR");
    g_ctx.f.surfaceSupport = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)g_ctx.gipa(inst, "vkGetPhysicalDeviceSurfaceSupportKHR");
    g_ctx.f.surfaceCaps = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)g_ctx.gipa(inst, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    g_ctx.f.surfaceFormats = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)g_ctx.gipa(inst, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    g_ctx.f.presentModes = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)g_ctx.gipa(inst, "vkGetPhysicalDeviceSurfacePresentModesKHR");
    g_ctx.getDevProc = (PFN_vkGetDeviceProcAddr)g_ctx.gipa(inst, "vkGetDeviceProcAddr");
    if (!g_ctx.f.qfProps || !g_ctx.f.props || !g_ctx.f.createDevice || !g_ctx.f.createSurface || !g_ctx.f.surfaceSupport ||
        !g_ctx.f.surfaceCaps || !g_ctx.f.surfaceFormats || !g_ctx.f.presentModes || !g_ctx.getDevProc) {
        *err = "instance functions missing";
        return false;
    }
    return true;
}

// picks the best GPU that has a graphics queue (discrete > integrated > other); records its name
bool vk_pick_device(const char** err) {
    auto enumPD = (PFN_vkEnumeratePhysicalDevices)g_ctx.gipa(g_ctx.inst, "vkEnumeratePhysicalDevices");
    if (!enumPD) { *err = "vkEnumeratePhysicalDevices missing"; return false; }
    PDev pds[8];
    uint32_t npd = 8;
    enumPD(g_ctx.inst, &npd, pds);
    PDev best = nullptr;
    uint32_t bestQf = 0;
    int bestScore = -1;
    for (uint32_t i = 0; i < npd; i++) {
        uint32_t nq = 0;
        g_ctx.f.qfProps(pds[i], &nq, nullptr);
        QueueFamilyProperties qp[16];
        if (nq > 16) nq = 16;
        g_ctx.f.qfProps(pds[i], &nq, qp);
        int fam = -1;
        for (uint32_t k = 0; k < nq; k++) if (qp[k].queueFlags & QF_GRAPHICS) { fam = (int)k; break; }
        if (fam < 0) continue;
        vk::PhysicalDeviceProperties p;
        g_ctx.f.props(pds[i], &p);
        int score = p.deviceType == vk::DEVICE_TYPE_DISCRETE ? 3 : p.deviceType == vk::DEVICE_TYPE_INTEGRATED ? 2 : 1;
        if (score > bestScore) {
            bestScore = score;
            best = pds[i];
            bestQf = (uint32_t)fam;
            for (int c = 0; c < 255 && p.deviceName[c]; c++) g_ctx.gpu[c] = p.deviceName[c];
            g_ctx.gpu[255] = 0;
        }
    }
    if (!best) { *err = "no GPU with a graphics queue"; return false; }
    g_ctx.pd = best;
    g_ctx.qf = bestQf;
    return true;
}

}}  // namespace evgx::vkp
