# evg — easy Vulkan GUI

evg is a zero-dependency C++17 library for opening windows, presenting Vulkan frames, drawing
with a software rasterizer, building Qt 6 interfaces, and loading common file formats. The core
uses no STL, no libc and no third-party code. Qt, OpenEXR and Vulkan are optional and only used
by the modules that need them.

```cpp
#include <evg/evg.hpp>

int main() {
    evgx::Window* w = evg.createwindow(1280, 720, "hello");
    while (w && !w->closed) {
        w->open();                                   // pump OS events, update dt and input state
        if (w->key_pressed('Q')) break;
        evg.vulkan_clear(w, 0.1f, 0.2f, 0.4f);       // GPU frame (Windows; 2 frames in flight)
    }
    evg.destroywindow(w);
}
```

## Features

| Area | What is there |
|---|---|
| Windows and input | Windows (Win32; headless on Linux), keyboard, mouse, wheel, resize, event queue |
| Vulkan | Instance, device, swapchain, pipelined clear and present, GPU cleanup on window close |
| Software rendering | Depth-tested triangles, clipping, Blinn-Phong or image-based lighting, BMP screenshots |
| Models | glTF 2.0 binary (`.glb`): meshes, PBR factors, scene graph, bounds |
| Textures | PNG (8/16-bit, palette, not interlaced), BMP, TGA; nearest and bilinear sampling |
| Math | GLM-compatible vectors, matrices, quaternions, GLSL helpers, no libm |
| Qt 6 | Windows, layouts, menus, dialogs, buttons, inputs, lists, spin boxes, tables, tabs, timers, settings |
| SDL2 shim | `SDL_Init`, `SDL_PollEvent`, keyboard and mouse state, timing (for code written against SDL2) |
| Helpers | strings, UTF-8, bits, hashing, random numbers, colors, arguments, logging, files, timers, arena, ring buffer, DEFLATE |

## Requirements

- C++17 compiler. Tested: MSVC 2022 (Windows 10/11, AMD Radeon iGPU, driver 31.0.21924).
- CMake 3.20+ and Ninja (or Visual Studio).
- Vulkan driver for the GPU paths. Without one, the Vulkan calls return false and the rest works.
- Optional: Qt 6.8 (`-DEVG_WITH_QT=ON`), OpenEXR (`-DEVG_WITH_EXR=ON`).

Linux and macOS are not tested. Linux has a headless window path (no display), so software
rendering and file loading work there. X11, Wayland and Cocoa windows are not implemented.

## Build

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release `
      -DEVG_WITH_QT=ON -DEVG_WITH_EXR=ON `
      -DCMAKE_PREFIX_PATH="C:/path/to/vcpkg/installed/x64-windows;C:/Qt/6.8.3/msvc2022_64"
cmake --build build --parallel 2      # Qt needs --parallel 2 on machines with 16 GB RAM
ctest --test-dir build --output-on-failure
```

Options:

| Option | Default | Meaning |
|---|---|---|
| `EVG_WITH_QT` | OFF | build the Qt 6 bridge (otherwise every Qt call returns an error) |
| `EVG_WITH_EXR` | OFF | OpenEXR environment map loading |
| `EVG_BUILD_TESTS` | ON | test executables and `ctest` registration |
| `EVG_BUILD_EXAMPLES` | ON | examples in `examples/` |
| `EVG_BUILD_TOOLS` | ON | command-line tools in `tools/` |

## Command-line tools

```text
evg_info model.glb [more.glb ...] [--json]
    prints primitives, instances, triangles, materials, load time and bounds.

evg_shot model.glb out.bmp [--size=640x360] [--yaw=40] [--pitch=20] [--bg=RRGGBB]
    renders the model with an orbit camera and writes a 24-bit BMP.
```

Flags: `--json` and `--help` take no value, so `evg_info --json model.glb` works in either order.

## Repository layout

```text
include/evg/        public headers (evg.hpp, math.hpp, texture.hpp, qt/, compat/, common/)
src/core/           windows, input, camera, model and render API, platform layer, allocator
src/render/         software rasterizer, GLB loader, lighting, textures, PNG
src/vulkan/         Vulkan context, device, swapchain, presentation, cleanup
src/qt/             Qt bridge, one file per widget family, plus a no-Qt stub
src/compat/         SDL2 shim
src/common/         file helpers, DEFLATE decoder
tests/              one executable per API area (ctest names start with test_)
examples/           small programs, each one runs to completion
tools/              evg_info and evg_shot
vcpkg/ports/evg/    overlay port for vcpkg (repository URL and hash are placeholders until a release)
docs/               API reference, changelog, sample images
```

## Documentation

- [API reference](docs/API.md): every public module with its functions.
- [Changelog](CHANGELOG.md): what changed in each version, and the known issues still open.

## License

MIT. See LICENSE.
