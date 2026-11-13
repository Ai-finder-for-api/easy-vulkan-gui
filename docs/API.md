# evg API reference (v1.0)

evg is a zero-dependency C++17 library. The core uses no STL, no libc or libm calls, and no third-party code; Vulkan is
loaded from the driver at runtime. Qt, OpenEXR and the SDL2 shim are optional layers you enable at build time.

All functions live on one global object:

```cpp
#include <evg/evg.hpp>
evg.createwindow(1280, 720, "title");
```

## Windows

| call | purpose |
|---|---|
| `Window* createwindow(w, h, title)` | open a window (Win32 on Windows; headless elsewhere) |
| `Window* attach(native, w, h)` | draw into a window someone else owns (Qt winId(), SDL HWND) |
| `destroywindow(w)` | close and free; attached windows are never destroyed by evg |
| `bool poll_event(Event&)` | next queued event: `KeyDown`, `KeyUp`, `MouseMotion`, `MouseDown`, `MouseUp`, `MouseWheel`, `Resize`, `Quit` |
| `window->open()` | pump OS events, update input state and `dt`; false once closed |
| `double time()`, `sleep(ms)` | monotonic clock and sleep |

Input fields on `Window`: `key_down(k)`, `key_pressed(k)`, `button(b)`, `mouse_x`, `mouse_y`, `dx`, `dy`, `wheel`, `dt`.
Key codes: letters and digits are uppercase ASCII (`'W'`), other keys are `evgx::key::Left`, `Enter`, `Esc`, ...

## Rendering

| call | purpose |
|---|---|
| `bool begin(w, r, g, b)`, `end()` | software frame: clear, draw, present |
| `camera(Camera)`, `view_proj(view, proj)` | camera for the software and Vulkan paths |
| `light(dir, color, ambient)` | directional light (used when no environment is set) |
| `draw(model, world)` | draw a loaded model |
| `bool screenshot(w, "x.bmp")` | save the software frame |
| `bool vulkan_clear(w, r, g, b)` | clear and present a window on the GPU (pipelined, two frames in flight) |
| `const char* gpu()` | the GPU name Vulkan uses |

Cameras and controllers: `Camera` (fly with WASD, Q/E, right-drag look) and `Orbit` (left-drag orbit, wheel zoom).

## Models and lighting

| call | purpose |
|---|---|
| `Model* loadglb(path)`, `loadglb_memory(data, size)` | glTF 2.0 binary: meshes, nodes, PBR factors, normals, vertex colors |
| `freemodel(m)` | release a model |
| `import_lighting(path.exr)` | image-based lighting from an OpenEXR environment (needs `EVG_WITH_EXR`) |
| `import_lighting_raw(rgb, w, h)` | the same from float data |
| `clear_lighting()`, `has_lighting()` | manage the environment |

## Math (GLM-compatible)

Include `<evg/compat/glm.hpp>` and use `glm::`: `vec2/3/4`, `mat2/3/4`, `quat`, `translate`, `rotate`, `scale`,
`lookAt`, `perspective` (OpenGL and `perspective_vk`), `ortho`, `inverse`, `transpose`, `determinant`, `normalize`,
`dot`, `cross`, `length`, `reflect`, `refract`, `mix`, `clamp`, `step`, `smoothstep`, `fract`, `mod`, `exp`, `log`, `pow`, `value_ptr`.

## SDL2 shim

Include `<evg/compat/SDL.h>` (put `include/evg/compat` first on the path to replace real SDL): `SDL_Init`,
`SDL_CreateWindow`, `SDL_PollEvent` (keyboard, mouse, wheel, resize, quit), `SDL_GetKeyboardState`, `SDL_GetMouseState`,
`SDL_GetRelativeMouseState`, `SDL_GetTicks`, `SDL_GetPerformanceCounter`/`Frequency`, `SDL_Delay`, `SDL_GetWindowSize`.

## Qt 6 (`-DEVG_WITH_QT=ON`)

`evg.qt.*` builds widgets from C-style calls. Every call works the same whether or not Qt is built; without Qt they fail
and `evg.error()` says why.

| call | purpose |
|---|---|
| `init()`, `pump()`, `quit()` | create the Qt application; process events (false when no Qt window is visible) |
| `createwindow(title, w, h)` | top-level window (host for child widgets) |
| `createbutton(parent, text, x, y, w, h)` | push button |
| `createlabel`, `createcheckbox`, `createlineedit` | label, check box, single-line text edit |
| `createslider(parent, min, max, x, y, w, h)` | horizontal slider |
| `createcanvas(parent, x, y, w, h)` | Vulkan-presented area; `canvas_window(c)` returns its evg `Window*` |
| `setclick(w, fn, user)`, `setchange(w, fn, user)` | callbacks: clicks; toggles, slider moves, text edits |
| `settext`, `gettext`, `setvalue`, `getvalue`, `setchecked`, `getchecked` | read and write widget state |
| `setenabled`, `setgeometry`, `show`, `hide`, `close`, `click` | layout and automation |

For custom drawing, subclass `evgx::qt::Canvas` (`include/evg/qt/evg_qt.hpp`) and override `frame(Window*)`. Keyboard and
mouse events from Qt are forwarded to the canvas's evg window.

### Qt features

| area | calls |
|---|---|
| layouts | `createbox(parent, vertical, x, y, w, h)`, `addtobox(box, child)`, `setcentral(window, widget)`, `setspacing`, `setmargins` |
| menus | `addmenu(window, "File")`, `addaction(menu, "&Save", "Ctrl+S", fn, user)`, `addseparator(menu)`, `triggeraction(menu, "Save")` |
| dialogs | `messagebox(parent, title, text, kind)` (0 info, 1 warning, 2 yes/no), `openfile(parent, title, filter)`, `savefile(...)` |
| timers | `createtimer(ms, fn, user)` returns an id; `stoptimer(id)` |
| settings | `setapp(org, app)`, `setsetting(key, value)`, `getsetting(key, fallback)` (QSettings, persisted) |
| background work | `runasync(work, user, done, duser)`: work on a thread pool, `done` on the GUI thread |
| tables | `createtable(parent, rows, cols, x, y, w, h)`, `settablecell`, `gettablecell`, `setheader` |
| style | `setstylesheet(widget, css)` |

Layout example:

```cpp
QtWidget* win = evg.qt.createwindow("app", 480, 320);
QtWidget* box = evg.qt.createbox(nullptr, true, 0, 0, 0, 0);   // vertical box
evg.qt.setcentral(win, box);
evg.qt.addtobox(box, evg.qt.createbutton(nullptr, "Open", 0, 0, 0, 0));
evg.qt.addtobox(box, evg.qt.createlabel(nullptr, "hello", 0, 0, 0, 0));
QtWidget* file = evg.qt.addmenu(win, "File");
evg.qt.addaction(file, "&Open...", "Ctrl+O", [](void*) { /* ... */ }, nullptr);
evg.qt.show(win);
```

Note: a widget created without a parent stays hidden until it is shown or added to a box.


Minimal app:

```cpp
#include <evg/evg.hpp>
evg.qt.init();
QtWidget* win = evg.qt.createwindow("app", 480, 320);
QtWidget* b = evg.qt.createbutton(win, "Click", 10, 10, 120, 30);
evg.qt.setclick(b, [](void* u) { /* ... */ }, nullptr);
evg.qt.show(win);
while (evg.qt.pump()) { }
```

## Build options

| option | default | effect |
|---|---|---|
| `EVG_BUILD_TESTS` | ON | test suites (`ctest`) |
| `EVG_BUILD_EXAMPLES` | ON | example programs |
| `EVG_WITH_EXR` | OFF | OpenEXR environment lighting |
| `EVG_WITH_QT` | OFF | Qt 6 widgets and canvas |

Layout: `include/` (public headers), `src/core` (window, input, platform), `src/render` (rasterizer, GLB, lighting),
`src/vulkan` (GPU backend), `src/compat` (SDL2 shim), `src/qt` (Qt bridge), `src/exr` (OpenEXR module), `tests/`,
`examples/`, `tools/` (asset generators), `vcpkg/` (overlay port), `docs/`.
