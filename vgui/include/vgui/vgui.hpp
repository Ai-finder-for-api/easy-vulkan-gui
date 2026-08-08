#pragma once

/**
 * @file vgui.hpp
 * @brief Main VGUI header - Vulkan-based GUI Library
 * 
 * VGUI is a modern, cross-platform GUI library built on Vulkan.
 * Supports Windows, Linux, and macOS with optional Metal backend.
 * 
 * @note This library requires Vulkan 1.3+ and C++20
 */

// Version information
#define VGUI_VERSION_MAJOR 1
#define VGUI_VERSION_MINOR 0
#define VGUI_VERSION_PATCH 0
#define VGUI_VERSION_STRING "1.0.0"

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define VGUI_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define VGUI_PLATFORM_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define VGUI_PLATFORM_MACOS
#endif

// Feature toggles
#define VGUI_ENABLE_ASSERTS
#define VGUI_USE_STD_SPINLOCK

// Include core headers
#include "vgui/core/application.hpp"
#include "vgui/core/window.hpp"
#include "vgui/core/renderer.hpp"
#include "vgui/core/context.hpp"
#include "vgui/core/config.hpp"
#include "vgui/core/version.hpp"
#include "vgui/core/logger.hpp"

// Include math headers
#include "vgui/math/vec2.hpp"
#include "vgui/math/vec3.hpp"
#include "vgui/math/vec4.hpp"
#include "vgui/math/mat4.hpp"
#include "vgui/math/quat.hpp"
#include "vgui/math/color.hpp"

// Include graphics headers
#include "vgui/graphics/texture.hpp"
#include "vgui/graphics/image.hpp"
#include "vgui/graphics/sampler.hpp"
#include "vgui/graphics/buffer.hpp"
#include "vgui/graphics/shader.hpp"
#include "vgui/graphics/pipeline.hpp"
#include "vgui/graphics/material.hpp"
#include "vgui/graphics/mesh.hpp"
#include "vgui/graphics/vertex.hpp"
#include "vgui/graphics/uniform.hpp"
#include "vgui/graphics/descriptor.hpp"
#include "vgui/graphics/framebuffer.hpp"
#include "vgui/graphics/renderpass.hpp"
#include "vgui/graphics/command.hpp"
#include "vgui/graphics/camera.hpp"

// Include scene headers
#include "vgui/scene/node.hpp"
#include "vgui/scene/scene.hpp"
#include "vgui/scene/transform.hpp"
#include "vgui/scene/entity.hpp"
#include "vgui/scene/light.hpp"
#include "vgui/scene/model.hpp"

// Include UI headers
#include "vgui/ui/canvas.hpp"
#include "vgui/ui/widget.hpp"
#include "vgui/ui/button.hpp"
#include "vgui/ui/label.hpp"
#include "vgui/ui/textbox.hpp"
#include "vgui/ui/panel.hpp"
#include "vgui/ui/layout.hpp"
#include "vgui/ui/font.hpp"

// Include utility headers
#include "vgui/util/timer.hpp"
#include "vgui/util/filesystem.hpp"
#include "vgui/util/uuid.hpp"
#include "vgui/util/profiler.hpp"

namespace vgui {

/**
 * @brief Initialize the VGUI library
 * @param config Optional configuration overrides
 * @return true on success, false on failure
 * 
 * Must be called before any other VGUI functions.
 * Automatically detects available Vulkan devices and picks the best one.
 */
inline bool init(const Config& config = Config::defaultConfig()) {
    return Context::getInstance().initialize(config);
}

/**
 * @brief Shutdown the VGUI library
 * 
 * Cleans up all allocated resources and terminates Vulkan.
 * Should be called when the application is exiting.
 */
inline void shutdown() {
    Context::getInstance().shutdown();
}

/**
 * @brief Check if VGUI is initialized
 */
inline bool isInitialized() {
    return Context::getInstance().isInitialized();
}

} // namespace vgui
