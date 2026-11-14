cmake_minimum_required(VERSION 3.20)
project(evg VERSION 1.0.0 LANGUAGES CXX)

option(EVG_BUILD_TESTS "Build the test suites" ON)
option(EVG_BUILD_EXAMPLES "Build the examples" ON)
option(EVG_BUILD_TOOLS "Build the command-line tools (evg_info, evg_shot)" ON)
option(EVG_WITH_EXR "evg.import_lighting() reads OpenEXR files (needs OpenEXR + Imath)" OFF)
option(EVG_WITH_QT "evg.qt.* widgets and the Qt canvas (needs Qt 6 Widgets; set CMAKE_PREFIX_PATH to the Qt install)" OFF)

# ---------------- library ----------------
# Every .cpp in a module folder is part of the library; platform files are chosen per OS below.
file(GLOB EVG_CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)
list(FILTER EVG_CORE_SOURCES EXCLUDE REGEX "platform_")
file(GLOB EVG_RENDER_SOURCES CONFIGURE_DEPENDS src/render/*.cpp)
file(GLOB EVG_VULKAN_SOURCES CONFIGURE_DEPENDS src/vulkan/*.cpp)
file(GLOB EVG_COMPAT_SOURCES CONFIGURE_DEPENDS src/compat/*.cpp)
file(GLOB EVG_COMMON_SOURCES CONFIGURE_DEPENDS src/common/*.cpp)
add_library(evg STATIC ${EVG_CORE_SOURCES} ${EVG_RENDER_SOURCES} ${EVG_VULKAN_SOURCES} ${EVG_COMPAT_SOURCES} ${EVG_COMMON_SOURCES})
add_library(evg::evg ALIAS evg)

if(WIN32)
  target_sources(evg PRIVATE src/core/platform_win32.cpp)
  target_link_libraries(evg PUBLIC user32)
else()
  target_sources(evg PRIVATE src/core/platform_posix.cpp)
  target_link_libraries(evg PUBLIC ${CMAKE_DL_LIBS})
endif()

if(EVG_WITH_QT)
  find_package(Qt6 REQUIRED COMPONENTS Widgets)
  file(GLOB EVG_QT_SOURCES CONFIGURE_DEPENDS src/qt/*.cpp)
  list(FILTER EVG_QT_SOURCES EXCLUDE REGEX "qt_bridge_stub")
  target_sources(evg PRIVATE ${EVG_QT_SOURCES})
  target_link_libraries(evg PRIVATE Qt6::Widgets)
else()
  target_sources(evg PRIVATE src/qt/qt_bridge_stub.cpp)
endif()

if(EVG_WITH_EXR)
  find_package(OpenEXR CONFIG REQUIRED)
  target_sources(evg PRIVATE src/exr/evg_exr.cpp)
  target_link_libraries(evg PRIVATE OpenEXR::OpenEXR)
  target_compile_definitions(evg PUBLIC EVG_WITH_EXR=1)
endif()

target_compile_features(evg PUBLIC cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(evg PRIVATE -fno-exceptions -fno-rtti -fno-stack-protector)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(evg PRIVATE -fno-tree-loop-distribute-patterns)   # keep our loops from becoming memset/memcpy calls
  endif()
endif()

target_include_directories(evg PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
# internal module directories (headers used across them; not part of the public API)
target_include_directories(evg PRIVATE src/core src/render src/vulkan src/compat src/qt src/exr)

# ---------------- install / package config ----------------
include(GNUInstallDirs)
install(TARGETS evg EXPORT evgTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION include)
install(EXPORT evgTargets NAMESPACE evg:: DESTINATION lib/cmake/evg)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/evgConfig.cmake
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/evgTargets.cmake\")\n")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/evgConfig.cmake DESTINATION lib/cmake/evg)

# ---------------- tests ----------------
if(EVG_BUILD_TESTS)
  enable_testing()
  # one test executable per file; the test name is the target name
  function(evg_add_test name src)
    add_executable(${name} ${src})
    target_link_libraries(${name} PRIVATE evg)
    target_compile_definitions(${name} PRIVATE
      EVG_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/assets"
      EVG_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
    add_test(NAME ${name} COMMAND ${name})
  endfunction()

  # math, core, compat, render: one file per API area
  evg_add_test(test_math_scalar   tests/math/test_scalar.cpp)
  evg_add_test(test_math_glsl     tests/math/test_glsl.cpp)
  evg_add_test(test_math_matrix   tests/math/test_matrix.cpp)
  evg_add_test(test_math_quat     tests/math/test_quat.cpp)
  evg_add_test(test_core_events   tests/core/test_events.cpp)
  evg_add_test(test_core_input    tests/core/test_input.cpp)
  evg_add_test(test_core_camera   tests/core/test_camera.cpp)
  evg_add_test(test_compat_sdl    tests/compat/test_sdl.cpp)
  evg_add_test(test_render_glb    tests/render/test_glb_loader.cpp)
  evg_add_test(test_render_frame  tests/render/test_frame.cpp)
  evg_add_test(test_render_texture tests/render/test_texture.cpp)
  evg_add_test(test_render_codec   tests/render/test_codec.cpp)
  evg_add_test(test_common_str_utf8         tests/common/test_str_utf8.cpp)
  evg_add_test(test_common_bits_hash_arena  tests/common/test_bits_hash_arena.cpp)
  evg_add_test(test_common_cli_color_log    tests/common/test_cli_color_log.cpp)
  evg_add_test(test_vk_cleanup              tests/vulkan/test_vk_cleanup.cpp)
  evg_add_test(test_vk_render               tests/vulkan/test_vk_render.cpp)

  # broader suites from earlier versions (still useful end-to-end checks)
  foreach(t math core vulkan lighting scene)
    evg_add_test(test_${t} tests/test_${t}.cpp)
  endforeach()
  if(EVG_WITH_QT)
    add_executable(test_qt tests/test_qt.cpp)
    target_link_libraries(test_qt PRIVATE evg Qt6::Widgets)
    add_test(NAME qt COMMAND test_qt)
    add_executable(test_qt_features tests/qt/test_qt_features.cpp)
    target_link_libraries(test_qt_features PRIVATE evg Qt6::Widgets)
    add_test(NAME qt_features COMMAND test_qt_features)
    add_executable(test_qt_widgets2 tests/qt/test_qt_widgets2.cpp)
    target_link_libraries(test_qt_widgets2 PRIVATE evg Qt6::Widgets)
    add_test(NAME qt_widgets2 COMMAND test_qt_widgets2)
    add_executable(test_qt_paint tests/qt/test_qt_paint.cpp)
    target_link_libraries(test_qt_paint PRIVATE evg Qt6::Widgets)
    add_test(NAME qt_paint COMMAND test_qt_paint)
  endif()
endif()

# ---------------- tools (command-line programs built on the helpers) ----------------
if(EVG_BUILD_TOOLS)
  foreach(t evg_info evg_shot)
    add_executable(${t} tools/${t}.cpp)
    target_link_libraries(${t} PRIVATE evg)
  endforeach()
endif()

# ---------------- examples ----------------
if(EVG_BUILD_EXAMPLES)
  foreach(e hello viewer vk_present render_glb ex_glb_info ex_orbit_shots ex_sdl_loop)
    add_executable(${e} examples/${e}.cpp)
    target_link_libraries(${e} PRIVATE evg)
  endforeach()
  if(EVG_WITH_QT)
    foreach(e qt_canvas qt_app ex_qt_layout ex_qt_background ex_qt_table)
      add_executable(${e} examples/${e}.cpp)
      target_link_libraries(${e} PRIVATE evg Qt6::Widgets)
    endforeach()
  endif()
endif()

