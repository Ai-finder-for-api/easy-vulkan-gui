# Changelog

## 0.2.0 (unreleased)

### Added
- Vulkan mesh rendering: `evg.vulkan_draw` draws a model into a window, `evg.vulkan_render_pixels` reads a GPU frame
  back as pixels. A render pass with color and depth, a graphics pipeline with push constants, per-size render
  targets, and per-model vertex and index buffers freed by `freemodel`. Shaders are GLSL compiled with `glslc`
  into `src/vulkan/mesh_spv.hpp` (regenerate with `tools/gen_shaders.ps1`).
- PNG loading (8/16-bit gray, gray+alpha, RGB, RGBA, 8-bit palette with tRNS). Chunk CRCs and the zlib Adler-32
  are verified. Interlaced files and bit depths 1, 2, 4 are rejected with a message.
- DEFLATE and zlib decoder (`evg/common/inflate.hpp`): stored, fixed and dynamic Huffman blocks, CRC-32 and Adler-32.
- BMP and TGA textures (`evg/texture.hpp`): loaders, BMP writer, nearest and bilinear sampling, repeat and clamp wrapping.
- Qt custom painting: `evg.qt.createpaint` draws with fill, stroke, line, ellipse, polygon, text, color, pen width,
  font size and antialiasing calls inside a paint callback. `requestpaint` schedules a repaint.
- Qt widgets: radio buttons, integer and decimal spin boxes, progress bars, multi-line text edits, combo boxes and
  list views (one item API), group boxes, tab sets, window status bar, read-only control. Change callbacks cover
  every input type.
- GPU cleanup: `destroywindow` releases the window's swapchain, synchronization objects and surface. Closing the
  last Vulkan window destroys the command pool, device and instance.
- Common helpers: strings (`str`), UTF-8, bit tricks, hashing (FNV, CRC-32), random numbers (xoshiro256**),
  colors, arena allocator, ring buffer, logging with a sink, stopwatch, file helpers.
- Command-line tools `evg_info` and `evg_shot`.
- Tests: 26 suites under `tests/` (22 without Qt). They include real PNG files from `docs/` and a GPU render test.

### Fixed
- The no-Qt build did not link: the stub file was missing every Qt function added since the widget work. The
  stub now defines the whole API.
- Argument parsing: `--json model.glb` no longer treats the file name as the value of `--json`. Pass boolean flags
  to `cli::Args` so they never take a value.
- Vulkan windows leaked their swapchain, semaphores, fences and surface when destroyed. They are released now.
- The window table kept a gap after a window was closed in the middle of the list. The table stays dense.
- Textures: a clamped fetch test checked the wrong texel (test expectation only; the code was correct).

### Changed
- `README.md` and `docs/API.md` rewritten for the current API; the Vulkan section describes mesh rendering.
- `evg_shot` and `evg_info` read their flags through the typed constructor.

## Known issues (open)

- **Mesh rendering on the GPU**: textures are not uploaded, environment lighting is not used, and the metallic and
  roughness terms are ignored. Each mesh call waits for the GPU, so frames are not pipelined on this path.
- **Presentation**: the clear path is pipelined; the mesh path is not. Only one render target is kept, so drawing
  to several windows of different sizes recreates it each time.
- **`.glb` textures**: materials do not load their textures yet. The CPU texture loaders work, but nothing samples
  them on the GPU.
- **`evg.gpu()`**: the device name is filled in by the first Vulkan call, not at startup.
- **Linux and macOS**: Linux has a headless window path only. macOS is not built or tested. The old repository
  claimed Linux, macOS and Android support; that code is in history and not in the current tree.
- **Windows minimum size**: the OS enlarges very small windows. `createwindow` returns the requested size, so read
  `width` and `height` after creation.
- **Qt**: the item model is a flat list (no `QAbstractItemModel`), and there is no docking. Widgets are created by
  handle, not by class. Custom painting covers the common primitives only (no paths, gradients or images yet).
- **Name clash**: `evgx::tex::free` shares a name with the C library function. Call it qualified.
- **Build**: the Qt build needs `--parallel 2` on machines with 16 GB RAM. The compiler runs out of memory with more
  jobs. On this machine the Windows SDK in `vcvars` (10.0.26100.0) is no longer installed; `env_build.bat` points at
  10.0.28000.0.
