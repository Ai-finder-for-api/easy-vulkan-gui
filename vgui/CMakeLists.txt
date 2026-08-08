cmake_minimum_required(VERSION 3.20)

project(vgui VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(VGUI_BUILD_EXAMPLES "Build example applications" ON)
option(VGUI_BUILD_TESTS "Build tests" OFF)
option(VGUI_ENABLE_VALIDATION "Enable Vulkan validation layers" OFF)
option(VGUI_USE_METAL "Enable Metal backend on macOS" ON)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Platform-specific dependencies
if(APPLE)
    if(VGUI_USE_METAL)
        find_library(METAL_LIBRARY Metal)
        find_library(METAL_FRAMEWORK MetalKit)
        set(VGUI_METAL_LIBRARIES ${METAL_LIBRARY} ${METAL_FRAMEWORK})
    endif()
elseif(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    find_package(EGL REQUIRED)
elseif(WIN32)
    # Windows uses Vulkan directly with Win32
endif()

# GLFW for window management (vcpkg: glfw3)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Core library sources
set(VGUI_CORE_SOURCES
    src/core/application.cpp
    src/core/window.cpp
    src/core/renderer.cpp
    src/core/context.cpp
    src/core/config.cpp
    src/core/logger.cpp
)

set(VGUI_BACKEND_SOURCES
    src/backend/common/backend.cpp
    src/backend/common/gpu.cpp
    src/backend/common/allocator.cpp
    src/backend/common/memory.cpp
)

set(VGUI_VULKAN_SOURCES
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

set(VGUI_GLFW_SOURCES
    src/backend/glfw/glfw_window.cpp
    src/backend/glfw/glfw_input.cpp
    src/backend/glfw/glfw_monitor.cpp
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

# Combine all sources
set(VGUI_ALL_SOURCES
    ${VGUI_CORE_SOURCES}
    ${VGUI_BACKEND_SOURCES}
    ${VGUI_VULKAN_SOURCES}
    ${VGUI_GLFW_SOURCES}
    ${VGUI_GRAPHICS_SOURCES}
    ${VGUI_SCENE_SOURCES}
    ${VGUI_UI_SOURCES}
    ${VGUI_MATH_SOURCES}
    ${VGUI_UTIL_SOURCES}
)

# Main VGUI library
add_library(vgui ${VGUI_ALL_SOURCES})

target_include_directories(vgui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(vgui
    PUBLIC
        Vulkan::Vulkan
        glfw
    PRIVATE
        ${CMAKE_DL_LIBS}
)

if(APPLE AND VGUI_USE_METAL)
    target_link_libraries(vgui PRIVATE ${VGUI_METAL_LIBRARIES})
endif()

if(VGUI_ENABLE_VALIDATION)
    target_compile_definitions(vgui PRIVATE VGUI_ENABLE_VALIDATION_LAYERS)
endif()

target_compile_definitions(vgui PRIVATE
    $<$<PLATFORM_ID:Windows>:VGUI_PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:VGUI_PLATFORM_LINUX>
    $<$<PLATFORM_ID:Darwin>:VGUI_PLATFORM_MACOS>
)

# Platform-specific compile definitions
if(WIN32)
    target_compile_definitions(vgui PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(vgui PRIVATE
        _POSIX_C_SOURCE=200809L
    )
endif()

# Install targets (vcpkg style)
include(GNUInstallDirs)
install(TARGETS vgui
    EXPORT vguiTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/vgui
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT vguiTargets
    FILE vguiTargets.cmake
    NAMESPACE vgui::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vgui
)

# Config file for find_package
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/vguiConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vgui
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vguiConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vgui
)

# Build examples
if(VGUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
