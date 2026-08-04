cmake_minimum_required(VERSION 3.15)
project(EasyVulkanGui VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

add_library(EasyVK STATIC
    src/Instance.cpp
    src/Window.cpp
)

target_include_directories(EasyVK PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(EasyVK PUBLIC Vulkan::Vulkan)

if(WIN32)
    target_compile_definitions(EasyVK PRIVATE VK_USE_PLATFORM_WIN32_KHR)
    target_link_libraries(EasyVK PUBLIC user32 gdi32)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(EasyVK PRIVATE VK_USE_PLATFORM_XCB_KHR)
    find_package(X11 REQUIRED)
    target_link_libraries(EasyVK PUBLIC ${X11_LIBRARIES})
    target_include_directories(EasyVK PUBLIC ${X11_INCLUDE_DIR})
endif()

include(GNUInstallDirs)

install(TARGETS EasyVK
    EXPORT EasyVKTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)