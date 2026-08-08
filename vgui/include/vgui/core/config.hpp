#pragma once

#include <string>
#include <optional>
#include "vgui/core/version.hpp"

namespace vgui {

/**
 * @brief Graphics API backend types
 */
enum class BackendType {
    Vulkan,
    Metal,        // macOS only
    OpenGL        // Fallback (not yet implemented)
};

/**
 * @brief Window creation hints
 */
struct WindowHints {
    std::string title = "VGUI Application";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool resizable = true;
    bool fullscreen = false;
    bool vsync = true;
    bool centered = true;
    
    // Vulkan-specific
    bool enableValidationLayers = false;
    
    // Mobile/Embedded hints
    bool preferIntegratedGPU = false;
    bool preferDiscreteGPU = false;
};

/**
 * @brief Configuration for VGUI initialization
 */
struct Config {
    Version version = VGUI_VERSION;
    
    // Backend selection
    BackendType preferredBackend = BackendType::Vulkan;
    bool forceMetalOnMac = false;  // Force Metal even if Vulkan available
    
    // Window configuration
    WindowHints window;
    
    // Vulkan instance configuration
    std::string applicationName = "VGUI App";
    uint32_t vulkanApiVersion = VK_API_VERSION_1_3;
    std::vector<const char*> requiredExtensions;
    std::vector<const char*> enabledDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_SAMPLED_IMAGE_LITERAL_BINDING_EXTENSION_NAME,
        VK_KHR_DRAW_MEMORY_PREFETCH_EXTENSION_NAME
    };
    
    // Validation layers (for debugging)
    bool enableValidation = false;
    std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    
    // Debug utils (for validation layer messages)
    bool enableDebugUtils = false;
    
    // Renderer settings
    uint32_t maxFrameLatency = 2;
    bool enableMsaa = false;
    uint32_t msaaSamples = 4;
    
    // Memory configuration
    bool preferDedicatedAllocation = true;
    bool useBindlessDescriptorSets = true;
    
    // UI configuration
    bool enableDebugUI = false;
    float defaultFontSize = 14.0f;
    
    /**
     * @brief Get default configuration
     */
    static Config defaultConfig() {
        Config config;
        config.applicationName = "VGUI Application";
        return config;
    }
    
    /**
     * @brief Set window parameters
     */
    Config& setWindow(const std::string& title, uint32_t width, uint32_t height) {
        window.title = title;
        window.width = width;
        window.height = height;
        return *this;
    }
    
    /**
     * @brief Enable or disable validation layers
     */
    Config& setValidation(bool enabled) {
        enableValidation = enabled;
        window.enableValidationLayers = enabled;
        return *this;
    }
    
    /**
     * @brief Enable MSAA
     */
    Config& setMsaa(bool enabled, uint32_t samples = 4) {
        enableMsaa = enabled;
        msaaSamples = samples;
        return *this;
    }
};

} // namespace vgui
