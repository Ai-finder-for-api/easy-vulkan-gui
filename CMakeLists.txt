cmake_minimum_required(VERSION 3.21)

# ╔══════════════════════════════════════════════════════════╗
# ║                        vgui                             ║
# ║   Zero-dependency cross-platform GPU GUI library        ║
# ╚══════════════════════════════════════════════════════════╝

project(vgui
    VERSION      0.1.0
    DESCRIPTION  "Zero-dependency cross-platform GPU-accelerated GUI library"
    HOMEPAGE_URL "https://github.com/yourusername/vgui"
    LANGUAGES    CXX
)

# ── Standard ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# ── Options ───────────────────────────────────────────────
option(VGUI_BUILD_EXAMPLES  "Build example programs"  ON)
option(VGUI_BUILD_TESTS     "Build unit tests"        OFF)
option(VGUI_VALIDATION      "Enable GPU validation"   OFF)

# Auto-select backend
if(APPLE)
    set(VGUI_BACKEND "Metal"  CACHE STRING "Rendering backend")
elseif(WIN32 OR UNIX)
    set(VGUI_BACKEND "Vulkan" CACHE STRING "Rendering backend")
endif()

message(STATUS "[vgui] Backend : ${VGUI_BACKEND}")
message(STATUS "[vgui] Platform: ${CMAKE_SYSTEM_NAME}")

# ── Sources ───────────────────────────────────────────────
set(VGUI_CORE_SOURCES
    src/core/application.cpp
    src/core/window.cpp
    src/core/renderer.cpp
    src/core/context.cpp
    src/core/config.cpp
    src/core/logger.cpp
)

set(VGUI_PLATFORM_SOURCES_WINDOWS
    src/platform/windows/win32_window.cpp
    src/platform/windows/win32_input.cpp
    src/platform/windows/win32_monitor.cpp
    src/platform/windows/win32_cursor.cpp
    src/platform/windows/win32_clipboard.cpp
)

set(VGUI_PLATFORM_SOURCES_LINUX
    src/platform/linux/x11_window.cpp
    src/platform/linux/x11_input.cpp
    src/platform/linux/x11_monitor.cpp
    src/platform/linux/wayland_window.cpp
    src/platform/linux/wayland_input.cpp
)

set(VGUI_PLATFORM_SOURCES_MACOS
    src/platform/macos/ns_window.mm
    src/platform/macos/ns_input.mm
    src/platform/macos/ns_monitor.mm
    src/platform/macos/ns_cursor.mm
    src/platform/macos/ns_clipboard.mm
)

set(VGUI_INPUT_SOURCES
    src/input/input_manager.cpp
    src/input/keyboard.cpp
    src/input/mouse.cpp
    src/input/gamepad.cpp
    src/input/touch.cpp
    src/input/event_queue.cpp
)

set(VGUI_WINDOW_SOURCES
    src/window/window_manager.cpp
    src/window/window_base.cpp
    src/window/cursor.cpp
    src/window/monitor.cpp
    src/window/clipboard.cpp
)

set(VGUI_BACKEND_COMMON_SOURCES
    src/backend/common/backend.cpp
    src/backend/common/gpu.cpp
    src/backend/common/allocator.cpp
    src/backend/common/memory.cpp
)

set(VGUI_BACKEND_VULKAN_SOURCES
    src/backend/vulkan/vk_instance.cpp
    src/backend/vulkan/vk_surface.cpp
    src/backend/vulkan/vk_physical_device.cpp
    src/backend/vulkan/vk_device.cpp
    src/backend/vulkan/vk_queue.cpp
    src/backend/vulkan/vk_swapchain.cpp
    src/backend/vulkan/vk_renderpass.cpp
    src/backend/vulkan/vk_framebuffer.cpp
    src/backend/vulkan/vk_pipeline.cpp
    src/backend/vulkan/vk_shader.cpp
    src/backend/vulkan/vk_buffer.cpp
    src/backend/vulkan/vk_image.cpp
    src/backend/vulkan/vk_texture.cpp
    src/backend/vulkan/vk_sampler.cpp
    src/backend/vulkan/vk_descriptor.cpp
    src/backend/vulkan/vk_command_pool.cpp
    src/backend/vulkan/vk_command_buffer.cpp
    src/backend/vulkan/vk_sync.cpp
    src/backend/vulkan/vk_upload.cpp
    src/backend/vulkan/vk_barrier.cpp
    src/backend/vulkan/vk_debug.cpp
    src/backend/vulkan/vk_renderer.cpp
)

set(VGUI_BACKEND_METAL_SOURCES
    src/backend/metal/mtl_device.mm
    src/backend/metal/mtl_swapchain.mm
    src/backend/metal/mtl_pipeline.mm
    src/backend/metal/mtl_buffer.mm
    src/backend/metal/mtl_texture.mm
    src/backend/metal/mtl_sampler.mm
    src/backend/metal/mtl_command.mm
    src/backend/metal/mtl_renderer.mm
)

set(VGUI_GRAPHICS_SOURCES
    src/graphics/texture.cpp
    src/graphics/image.cpp
    src/graphics/sampler.cpp
    src/graphics/buffer.cpp
    src/graphics/shader.cpp
    src/graphics/pipeline.cpp
    src/graphics/material.cpp
    src/graphics/mesh.cpp
    src/graphics/descriptor.cpp
    src/graphics/framebuffer.cpp
    src/graphics/renderpass.cpp
    src/graphics/command.cpp
    src/graphics/camera.cpp
)

set(VGUI_SCENE_SOURCES
    src/scene/scene.cpp
    src/scene/entity.cpp
    src/scene/transform.cpp
    src/scene/node.cpp
    src/scene/light.cpp
    src/scene/model.cpp
)

set(VGUI_UI_SOURCES
    src/ui/canvas.cpp
    src/ui/widget.cpp
    src/ui/button.cpp
    src/ui/label.cpp
    src/ui/textbox.cpp
    src/ui/panel.cpp
    src/ui/layout.cpp
    src/ui/font.cpp
)

set(VGUI_MATH_SOURCES
    src/math/vec2.cpp
    src/math/vec3.cpp
    src/math/vec4.cpp
    src/math/mat4.cpp
    src/math/quat.cpp
    src/math/color.cpp
)

set(VGUI_UTIL_SOURCES
    src/util/timer.cpp
    src/util/filesystem.cpp
    src/util/uuid.cpp
    src/util/profiler.cpp
)

# ── Assemble sources ───────────────────────────────────────
set(VGUI_ALL_SOURCES
    ${VGUI_CORE_SOURCES}
    ${VGUI_INPUT_SOURCES}
    ${VGUI_WINDOW_SOURCES}
    ${VGUI_BACKEND_COMMON_SOURCES}
    ${VGUI_GRAPHICS_SOURCES}
    ${VGUI_SCENE_SOURCES}
    ${VGUI_UI_SOURCES}
    ${VGUI_MATH_SOURCES}
    ${VGUI_UTIL_SOURCES}
)

if(WIN32)
    list(APPEND VGUI_ALL_SOURCES ${VGUI_PLATFORM_SOURCES_WINDOWS})
elseif(APPLE)
    list(APPEND VGUI_ALL_SOURCES ${VGUI_PLATFORM_SOURCES_MACOS})
elseif(UNIX)
    list(APPEND VGUI_ALL_SOURCES ${VGUI_PLATFORM_SOURCES_LINUX})
endif()

if(VGUI_BACKEND STREQUAL "Vulkan")
    list(APPEND VGUI_ALL_SOURCES ${VGUI_BACKEND_VULKAN_SOURCES})
elseif(VGUI_BACKEND STREQUAL "Metal")
    list(APPEND VGUI_ALL_SOURCES ${VGUI_BACKEND_METAL_SOURCES})
endif()

# ── Library ────────────────────────────────────────────────
add_library(vgui ${VGUI_ALL_SOURCES})
add_library(vgui::vgui ALIAS vgui)

target_include_directories(vgui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# ── Platform links (OS-provided, zero external deps) ──────
if(WIN32)
    target_link_libraries(vgui PRIVATE user32 gdi32 dwmapi hid)
    target_compile_definitions(vgui PUBLIC
        VGUI_PLATFORM_WINDOWS
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
    )
elseif(APPLE)
    target_link_libraries(vgui PRIVATE
        "-framework Cocoa"
        "-framework Metal"
        "-framework MetalKit"
        "-framework QuartzCore"
        "-framework IOKit"
        "-framework CoreFoundation"
        "-framework CoreGraphics"
    )
    target_compile_definitions(vgui PUBLIC VGUI_PLATFORM_MACOS)
elseif(UNIX)
    find_package(X11 REQUIRED)
    target_link_libraries(vgui PRIVATE ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
    target_compile_definitions(vgui PUBLIC VGUI_PLATFORM_LINUX)
endif()

if(VGUI_BACKEND STREQUAL "Vulkan")
    find_package(Vulkan REQUIRED)
    target_link_libraries(vgui PUBLIC Vulkan::Vulkan)
    target_compile_definitions(vgui PUBLIC VGUI_BACKEND_VULKAN)
endif()

if(VGUI_BACKEND STREQUAL "Metal")
    target_compile_definitions(vgui PUBLIC VGUI_BACKEND_METAL)
endif()

if(VGUI_VALIDATION)
    target_compile_definitions(vgui PUBLIC VGUI_VALIDATION_ENABLED)
endif()

# ── Compiler warnings ──────────────────────────────────────
if(MSVC)
    target_compile_options(vgui PRIVATE /W4)
else()
    target_compile_options(vgui PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ── Examples ───────────────────────────────────────────────
if(VGUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ── Install ────────────────────────────────────────────────
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS vgui
    EXPORT      vguiTargets
    ARCHIVE     DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY     DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME     DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT vguiTargets
    FILE        vguiTargets.cmake
    NAMESPACE   vgui::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vgui
)

configure_package_config_file(
    cmake/vguiConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vgui
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfigVersion.cmake
    VERSION     ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vgui
)
