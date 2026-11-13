# evg API reference

Every public header lives under `include/evg/`. Include what you use: `<evg/evg.hpp>` brings the
window, render, model and Qt entry points; the rest are opt-in. Names are in the `evgx` namespace,
and the global object `evg` is the entry point to the window, render and Vulkan calls.

Errors: most calls return `bool`, a pointer or `-1`. After a failure, `evg.error()` returns a
message describing it (valid until the next call).

---

## 1. Windows and frames (`evg.hpp`)

```cpp
evgx::Window* evg.createwindow(int w, int h, const char* title);   // Win32 window with a software framebuffer
evgx::Window* evg.attach(void* native_hwnd, int w, int h);        // draw into a window the caller owns
void          evg.destroywindow(evgx::Window* w);                  // releases GPU objects, then the window
bool          evg.poll_event(evgx::Event& e);                      // next queued event, false when empty
void          evg.sleep(int ms);
double        evg.time();                                          // monotonic seconds
```

`Window` fields you read directly: `width`, `height`, `fbw`, `fbh` (framebuffer), `closed`,
`mouse_x`, `mouse_y`, `dx`, `dy`, `wheel`, `dt` (seconds, clamped to 0.1), `native` (OS handle).

Events (`evgx::Event`): `type` is one of `Quit`, `KeyDown`, `KeyUp`, `MouseMotion`, `MouseDown`,
`MouseUp`, `MouseWheel`, `Resize`. The queue holds 256 events; when it is full, new events are dropped.

Windows: the OS enforces a minimum client size, so a very small `createwindow` request can come
back larger. Read `w->width` and `w->height` after creation.

### Software rendering

```cpp
bool begin(Window* w, float r, float g, float b);   // clears the framebuffer and depth; false if closed or minimized
void end();
void camera(const Camera& c);                         // sets view and projection from the window aspect
void view_proj(const math::mat4& view, const math::mat4& proj);   // OpenGL clip conventions
void light(const math::vec3<float>& direction, const math::vec3<float>& color, float ambient);
void draw(const Model* m, const math::mat4& world = math::mat4());
bool screenshot(Window* w, const char* path_bmp);    // last frame as a 24-bit BMP
```

Drawing outside `begin`/`end` does nothing.

---

## 2. Input and cameras (`evg.hpp`)

```cpp
bool  w->key_down(int key);        // held
bool  w->key_pressed(int key);     // went down this frame
bool  w->button(int b);            // 0 left, 1 right, 2 middle
void  w->open();                   // starts a frame: edge flags and wheel reset, dt updates
```

Keys: uppercase ASCII for letters and digits, and `evgx::key::Left/Right/Up/Down/Enter/Esc/Space/Shift/Ctrl/Tab`.

Cameras:

```cpp
struct Camera { math::vec3 pos; float yaw, pitch; math::vec3 forward() const; void fly(Window& w, float speed); };
struct Orbit  { math::vec3 target; float dist, yaw, pitch;
                void update(Window& w); void frame(const Model* m); Camera camera() const; };
```

`Camera::fly` moves with W/A/S/D and Q/E (or Ctrl/Space for down/up); Shift triples the speed; drag
the right mouse button to look. `Orbit::update` zooms with the wheel and rotates with the left button.
`Orbit::frame` centers on a model's bounds. `Camera` also provides `view()` and `proj(aspect)`;
`Orbit::camera()` returns a `Camera` you pass to `evg.camera`.

---

## 3. Models (`evg.hpp`, `glb.hpp`)

```cpp
Model* evg.loadglb(const char* path);
Model* evg.loadglb_memory(const void* data, size_t n);
void   evg.freemodel(Model* m);
```

A loaded `Model` has `nprims`, `ninst`, `nmats`, `skipped` (primitives with unsupported modes),
`bmin`/`bmax` (world bounds), and the `prims`, `inst` and `mats` arrays.

Supported: GLB 2.0 with triangle lists, `POSITION`, `NORMAL`, `COLOR_0`, indexed or not, and PBR
factors (`baseColorFactor`, `metallicFactor`, `roughnessFactor`, `emissiveFactor`, `doubleSided`).
Not supported: skinning, animation, sparse accessors, external buffers, and texture images in
materials (the textures module below loads them separately).

---

## 4. Textures (`texture.hpp`)

```cpp
namespace evgx::tex {
struct Texture { int w, h; uint32_t* pixels; bool valid() const; };   // pixels are 0xAARRGGBB, top row first
enum class Wrap { Repeat, Clamp };

bool create(Texture& t, int w, int h, uint32_t color = 0xFFFFFFFF);
void free(Texture& t);

bool load_png(Texture& t, const char* path, const char** err);      // also load_png_memory
bool load_bmp(Texture& t, const char* path, const char** err);      // also load_bmp_memory
bool load_tga(Texture& t, const char* path, const char** err);      // also load_tga_memory
bool save_bmp(const Texture& t, const char* path);

uint32_t fetch(const Texture& t, int x, int y, Wrap = Wrap::Repeat);
math::vec4<float> sample_nearest(const Texture& t, float u, float v, Wrap = Wrap::Repeat);
math::vec4<float> sample_bilinear(const Texture& t, float u, float v, Wrap = Wrap::Repeat);
}
```

PNG: 8 and 16-bit gray, gray+alpha, RGB and RGBA, 8-bit palette with `tRNS`. Chunk CRCs and
the zlib Adler-32 are checked. Interlaced files and bit depths 1, 2, 4 are rejected.
BMP: 24 and 32-bit uncompressed. TGA: truecolor (types 2, 10) and grayscale 8-bit (3, 11).

Textures are CPU-side. There is no GPU upload yet.

---

## 5. Vulkan (`evg.hpp`)

```cpp
bool  evg.vulkan_clear(Window* w, float r, float g, float b);   // one frame, presented; 2 in flight
const char* evg.gpu();                                           // device name, after the first Vulkan call
```

Windows own their swapchain, surface and synchronization objects. `destroywindow` waits for the
device to go idle and frees them. Closing the last Vulkan window also destroys the command pool,
device and instance, so the next window starts from a clean context.

Not yet: pipelines, shaders, buffers, textures on the GPU, depth buffer on the GPU.

---

## 6. Lighting (`lighting.hpp`)

Environment light, through the `evg` object (`evg.import_lighting` and friends):

```cpp
bool evg.import_lighting(const char* exr_path);            // requires EVG_WITH_EXR; false otherwise
bool evg.import_lighting_raw(const float* rgb, int w, int h);   // linear RGB, no file needed
void evg.clear_lighting();
bool evg.has_lighting() const;
```

Environment light: spherical-harmonic irradiance for diffuse, and a roughness-prefiltered mip chain for specular.

---

## 7. Math (`math.hpp`, `compat/glm.hpp`)

Own implementations of everything: no libm. Include `<evg/compat/glm.hpp>` to use the names with
the `glm::` prefix.

- Types: `vec2/3/4`, `mat2/3/4`, `quat`, with float, double, int and unsigned variants of vectors.
- Transforms: `translate`, `scale`, `rotate`, `lookAt`, `perspective` (OpenGL clip), `perspective_vk`
  (Vulkan clip), `ortho` (4 and 6 argument).
- Matrix: `inverse` (general 4x4, and 2x2 and 3x3), `transpose`, `determinant`, `value_ptr`, `mat4_cast`, `mat3_cast`.
- Quaternion: `angleAxis`, `slerp`, `normalize`, `conjugate`.
- Scalar: `sin`, `cos`, `tan`, `atan`, `atan2`, `asin`, `acos`, `sqrt`, `exp`, `log`, `pow`,
  `floor`, `ceil`, `fract`, `mod`, `sign`, `step`, `smoothstep`, `min`, `max`, `clamp`, `mix`, `radians`, `degrees`.
- Vector: `dot`, `cross`, `length`, `distance`, `reflect`, `refract`, component-wise overloads.

Accuracy: sin, cos below 2e-7 absolute, sqrt about 1e-7 relative, exp and pow about 1e-6 relative
(checked against the C library in `tests/math`).

---

## 8. Qt 6 (`qt/evg_qt.hpp`, requires `EVG_WITH_QT`)

All calls live under `evg.qt`. Every function that creates a widget takes an optional `parent`
(`nullptr` for a top-level window) and a geometry `x, y, w, h`. Widgets with a parent show
themselves; use `show` for top-level windows.

| Group | Functions |
|---|---|
| Application | `init`, `pump` (returns false when no window is visible), `quit`, `createwindow(title, w, h)` |
| Buttons and labels | `createbutton`, `createlabel`, `setclick`, `click` |
| Check and radio | `createcheckbox`, `createradio`, `setchecked`, `getchecked` |
| Text | `createlineedit`, `createtextedit`, `settext`, `gettext`, `setreadonly` |
| Numbers | `createslider`, `createspinbox`, `createdoublespinbox`, `createprogress`, `setvalue`, `getvalue`, `setdoublevalue`, `getdoublevalue` |
| Item lists | `createcombo`, `createlist`, `additem`, `clearitems`, `setcurrentindex`, `getcurrentindex`, `getcurrenttext` |
| Tables | `createtable`, `settablecell`, `gettablecell`, `setheader` |
| Containers | `createbox`, `addtobox`, `setcentral`, `setspacing`, `setmargins`, `creategroup`, `createtabs`, `addtab`, `setstatus` |
| Menus | `addmenu`, `addaction`, `addseparator`, `triggeraction` |
| Dialogs | `messagebox`, `openfile`, `savefile` |
| Timers | `createtimer`, `stoptimer` |
| Settings | `setapp`, `setsetting`, `getsetting` |
| Background work | `runasync(work, user, done, duser)` (completion on the GUI thread) |
| Style and geometry | `setstylesheet`, `setenabled`, `setgeometry`, `show`, `hide`, `close`, `isvisible` |
| Events | `setchange` (value, text, selection or toggle changes, per widget type) |
| Canvas | `createcanvas`, `canvas_window` (a Vulkan-presented area inside the UI) |

Widget handles are opaque `QtWidget*` values. Strings returned by `gettext`, `getcurrenttext`,
`gettablecell`, `getsetting`, `openfile` and `savefile` are valid until the next call of the same function.

---

## 9. SDL2 shim (`compat/SDL.h`)

For code written against SDL2. Covers `SDL_Init`, `SDL_Quit`, `SDL_GetError`, `SDL_CreateWindow`,
`SDL_DestroyWindow`, `SDL_GetWindowSize`, `SDL_PollEvent` (keyboard, mouse, wheel, quit, resize),
`SDL_GetKeyboardState`, `SDL_GetMouseState`, `SDL_GetRelativeMouseState`, `SDL_SetRelativeMouseMode`,
`SDL_GetRelativeMouseMode`, `SDL_ShowCursor`, `SDL_GetTicks`, `SDL_GetPerformanceCounter`,
`SDL_GetPerformanceFrequency`, `SDL_Delay`. Windows are evg windows. Audio, joysticks, and text
input beyond the active flag are not implemented.

---

## 10. Common helpers (`include/evg/common/`)

All header-only except `file` and `inflate`, which are compiled into the library.

| Header | Contents |
|---|---|
| `str.hpp` | `str::View` (non-owning string), `equal`, `starts_with`, `ends_with`, `find`, `trim`, `split`, `to_i64`, `to_f64`, `format_i64` |
| `utf8.hpp` | `decode`, `encode`, `count`, `valid` (invalid bytes become U+FFFD) |
| `bits.hpp` | `popcount32/64`, `clz32`, `ctz32`, `is_pow2`, `next_pow2`, `align_up`, `rotl32/rotr32/rotl64`, `bswap32` |
| `hash.hpp` | `fnv1a32`, `fnv1a64`, `crc32` (IEEE, streamable with a seed), `combine` |
| `random.hpp` | `splitmix64`, `rnd::Rng` (xoshiro256**; `next`, `uniform`, `range`; deterministic per seed; not cryptographic) |
| `color.hpp` | `pack`, `red/green/blue/alpha`, `to_u8`, `to_float`, `srgb_to_linear`, `linear_to_srgb`, `hsv_to_rgb`, `lerp` |
| `arena.hpp` | `mem::Arena` over a caller buffer: `alloc`, `alloc_n<T>`, `mark`, `rewind`, `reset`, `peak` |
| `ring.hpp` | `ds::Ring<T, N>`: `push` (refuses when full), `push_overwrite`, `pop`, `at`, `size` |
| `args.hpp` | `cli::Args`: `--flag`, `--key=value`, `--key value`, `-x`, positionals, `--`. Pass boolean flags to the constructor |
| `log.hpp` | `log::set_sink`, `log::debug/info/warn/error`. Nothing is printed until a sink is installed |
| `timer.hpp` | `timer::Stopwatch`, `timer::Scoped` |
| `file.hpp` | `file::size`, `file::read_all`, `file::write_all`, `file::free_buffer` |
| `inflate.hpp` | `inflate::zlib_decompress`, `deflate_decompress`, `crc32_ieee`, `adler32` |

Example:

```cpp
#include <evg/common/args.hpp>
#include <evg/common/str.hpp>

static const char* const kFlags[] = {"json", "help"};
evgx::cli::Args args(argc, argv, kFlags);
if (args.has("json")) { /* ... */ }
int n = args.get_int("count", 10);
```
