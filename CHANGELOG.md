# Changelog

## 0.2.0 (unreleased)

### Added
- PNG loading (8/16-bit gray, gray+alpha, RGB, RGBA, 8-bit palette with tRNS). Chunk CRCs and the zlib
  Adler-32 are verified. Interlaced files and bit depths 1, 2, 4 are rejected with a message.
- DEFLATE and zlib decoder (`evg/common/inflate.hpp`): stored, fixed and dynamic Huffman blocks,
  CRC-32 and Adler-32 helpers.
- BMP and TGA textures (`evg/texture.hpp`): loaders, BMP writer, nearest and bilinear sampling, repeat and clamp wrapping.
- Qt widgets: radio buttons, integer and decimal spin boxes, progress bars, multi-line text edits,
  combo boxes and list views (one item API), group boxes, tab sets, window status bar, read-only control.
  Change callbacks cover every input type.
- GPU cleanup: `destroywindow` releases the window's swapchain, synchronization objects and surface. Closing the
  last Vulkan window destroys the command pool, device and instance.
- Common helpers: strings (`str`), UTF-8, bit tricks, hashing (FNV, CRC-32), random numbers (xoshiro256**),
  colors, arena allocator, ring buffer, logging with a sink, stopwatch, file helpers.
- Command-line tools `evg_info` and `evg_shot`.
- Tests: 24 suites under `tests/`, including real PNG files from `docs/`.

### Fixed
- Argument parsing: `--json model.glb` no longer treats the file name as the value of `--json`. Pass
  boolean flags to `cli::Args` so they never take a value.
- Vulkan windows leaked their swapchain, semaphores, fences and surface when destroyed. They are released now.
- The window table kept a gap after a window was closed in the middle of the list. The table stays dense.
- Textures: a clamped fetch test checked the wrong texel (test expectation only; the code was correct).

### Changed
- `README.md` and `docs/API.md` rewritten for the current API.
- `evg_shot` and `evg_info` read their flags through the typed constructor.

## Known issues (open)

- **Vulkan rendering**: presentation clears only. No shaders, pipelines, vertex or index buffers,
  textures on the GPU, or depth buffer on the GPU.
- **GPU textures**: texture images are CPU-side. Materials in `.glb` files do not load their textures yet.
- **`evg.gpu()`**: the device name is filled in by the first Vulkan call, not at startup.
- **Linux and macOS**: Linux has a headless window path only. macOS is not built or tested. The old
  repository claimed Linux, macOS and Android support; that code is in history and not in the current tree.
- **Windows minimum size**: the OS enlarges very small windows. `createwindow` returns the requested size, so read
  `width` and `height` after creation.
- **Qt**: the item model is a flat list (no `QAbstractItemModel`), no custom painting, and no docking. Widgets are
  created by handle, not by class.
- **Name clash**: `evgx::tex::free` shares a name with the C library function. Call it qualified.
- **Tests**: Vulkan tests skip cleanly without a driver, but the GPU cleanup test does not measure leaks;
  it checks that the paths run without failing.
- **Build**: the Qt build needs `--parallel 2` on machines with 16 GB RAM. The compiler runs out of memory with more jobs.
