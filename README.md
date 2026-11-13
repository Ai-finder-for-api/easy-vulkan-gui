# evg: easy Vulkan GUI

A zero-dependency C++17 library for windows, input, the GPU, glTF models and Qt 6 widgets, all behind one simple API:

```cpp
#include <evg/evg.hpp>

int main() {
    evgx::Window* w = evg.createwindow(1280, 720, "hello");
    while (w->open()) {
        if (w->key_pressed('Q')) break;
        evg.vulkan_clear(w, 0.1f, 0.2f, 0.4f);   // presented on the GPU
    }
    evg.destroywindow(w);
}
```

Full reference: [docs/API.md](docs/API.md).

## What is in it

- **Windows and input:** Win32 windows, an event queue (keys, mouse, wheel, resize, quit), attach to windows owned by others.
- **GPU presentation:** Vulkan loaded at runtime (no SDK), swapchains, two frames in flight, mailbox or immediate present.
- **Software rendering:** rasterizer with depth, clipping, culling, lighting, a camera and fly/orbit controllers.
- **Models:** glTF 2.0 binary (`.glb`) loader, hardened against malformed files by fuzzing under sanitizers.
- **Lighting:** OpenEXR environment maps with spherical-harmonic irradiance and prefiltered specular (optional module).
- **Math:** GLM-compatible vectors, matrices, quaternions and GLSL helpers, with our own `sin`, `cos`, `exp`, `log`.
- **Compatibility:** an SDL2 shim (events, keyboard, mouse, timing) and Qt 6 widgets (`evg.qt.*`) with forwarded input.

## Build

```
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build
```

Options: `-DEVG_WITH_EXR=ON` for OpenEXR, `-DEVG_WITH_QT=ON` for Qt 6 (set `CMAKE_PREFIX_PATH` to the Qt install).

Examples: `hello`, `viewer` (orbit a `.glb`), `render_glb` (headless render), `vk_present` (GPU frame rate),
`qt_canvas`, `qt_app` (Qt widgets).

## Layout

```
include/evg/        public headers (evg.hpp, math.hpp, scene.hpp, lighting.hpp, compat/, qt/)
src/core/           window, input, event queue, platform (Win32, POSIX)
src/render/         software rasterizer, GLB loader, lighting
src/vulkan/         GPU backend: swapchain, frames in flight, readback
src/compat/         SDL2 shim
src/qt/             Qt bridge (and the stub used without Qt)
src/exr/            OpenEXR reader (optional)
tests/              math, core, vulkan, lighting, scene, qt suites
examples/           sample programs
tools/              dev-time asset generators (not needed to build or use evg)
vcpkg/ports/evg/    overlay port for vcpkg (fill in the repo and hash first)
docs/               API reference and images
```

## Status

Working on Windows with an AMD iGPU: Vulkan presentation, Win32 and Qt windows, Qt widgets and input, the software renderer,
GLB loading, and the test suites. Not yet: textures (PNG/JPEG decoding), skinning and animation, Vulkan scene drawing with
shaders, X11/Wayland windows, the sqlite3 layer, and freeing GPU resources when windows close.

## License

MIT (placeholder): choose your license before publishing. See LICENSE.
