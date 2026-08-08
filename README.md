# vgui

> Zero-dependency cross-platform GPU-accelerated GUI library.  
> **Vulkan 1.3** on Windows & Linux · **Metal 3** on macOS  
> Custom Win32 / X11 / NSWindow · No GLFW, no external deps

---

## Features

| Category      | Details                                                               |
|---------------|-----------------------------------------------------------------------|
| **Backends**  | Vulkan 1.3 (Win/Linux), Metal 3 (macOS)                              |
| **Window**    | Custom Win32 · X11+Wayland · NSWindow — zero GLFW                    |
| **Input**     | Keyboard · Mouse · Gamepad (XInput/evdev) · Touch · EventQueue       |
| **UI**        | Button · Label · Textbox · Panel · Layout (VStack/HStack/Grid/Flex)  |
| **Graphics**  | Mesh · Texture · Material (PBR) · Shader · Pipeline · Camera         |
| **Scene**     | Entity-component · Node graph · Lights · glTF 2.0 loader             |
| **Math**      | Vec2/3/4 · Mat4 · Quat · Color — header-only, constexpr              |
| **Util**      | Timer · UUID · Filesystem · Profiler                                  |
| **Deps**      | **None** — only OS-provided APIs (Vulkan SDK, Metal, X11, Win32)     |

---

## Quick Start

```cpp
#include <vgui/vgui.hpp>

int main() {
    auto& win   = vgui::make_screen("Hello vgui", 1280, 720);
    win.set_vsync(true);

    auto& btn   = vgui::make_button("Click me!");
    btn.x       = 560;
    btn.y       = 340;
    btn.width   = 160;
    btn.height  = 40;
    btn.on_click = []{ vgui::quit(); };

    vgui::Application::get().on_update = [](float dt) {
        if (vgui::Keyboard::get().pressed(vgui::Key::Escape))
            vgui::quit();
    };

    vgui::run();
}
```

---

## Building

### Prerequisites

| Platform | Required                                            |
|----------|-----------------------------------------------------|
| Windows  | Visual Studio 2022 + Vulkan SDK                     |
| Linux    | GCC 13 / Clang 16 + Vulkan SDK + `libX11-dev`       |
| macOS    | Xcode 15 + macOS 13 SDK (Metal included)            |

### With vcpkg (recommended)

```bash
# 1. Install vcpkg
git clone https://github.com/microsoft/vcpkg && cd vcpkg && bootstrap-vcpkg.sh

# 2. Clone vgui
git clone https://github.com/Ai-finder-for-api/vgui && cd vgui

# 3. Configure + build
cmake -B build \
      -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DVGUI_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

### Without vcpkg

```bash
cmake -B build -DVGUI_BUILD_EXAMPLES=ON
cmake --build build --config Release
```

vgui has **zero runtime dependencies** — no external libraries are required.  
The Vulkan SDK provides `vulkan-1.lib` / `libvulkan.so` which is already
present on any machine with a GPU driver installed.

---

## Easy Syntax

```cpp
// Window
auto& win    = vgui::make_screen("My App");
win.set_size(1920, 1080);
win.set_title("Updated Title");
win.set_fullscreen(true);

// UI
auto& btn    = vgui::make_button("Submit");
btn.x = 40;  btn.y = 40;
btn.width = 120;  btn.height = 36;
btn.normal_color = vgui::Color::from_hex(0x2980B9FF);
btn.on_click     = []{ /* ... */ };

auto& lbl    = vgui::make_label("Hello!");
lbl.font_size = 18.f;
lbl.color     = vgui::Color::white();

auto& tb     = vgui::make_textbox("Enter name...");
tb.on_submit = [](const std::string& s){ /* ... */ };

// Layout
auto& col    = vgui::make_layout(vgui::LayoutType::VStack);
col.gap      = 8;

// Input
if (vgui::Keyboard::get().pressed(vgui::Key::Space)) { /* ... */ }
if (vgui::Mouse::get().is_down(vgui::MouseButton::Left)) { /* ... */ }
vgui::Vec2 mp = vgui::Mouse::get().position();

// Math
vgui::Vec3 v  = vgui::Vec3::up();
vgui::Mat4 m  = vgui::Mat4::perspective(60.f * 3.14159f/180.f, 16.f/9.f, 0.01f, 1000.f);
vgui::Color c = vgui::Color::from_hex(0xFF6030FF);
vgui::Color d = c.lerp(vgui::Color::white(), 0.3f);
```

---

## Project Layout

```
vgui/
├── include/vgui/       Public headers  (install target)
│   ├── vgui.hpp        Single-include façade
│   ├── core/           Application · Window · Renderer · Logger
│   ├── input/          Keyboard · Mouse · Gamepad · Touch · EventQueue
│   ├── window/         WindowBase · WindowManager · Monitor · Cursor · Clipboard
│   ├── graphics/       Buffer · Texture · Mesh · Material · Pipeline · Camera
│   ├── scene/          Entity · Node · Transform · Scene · Light · Model
│   ├── ui/             Canvas · Widget · Button · Label · Textbox · Panel · Layout · Font
│   ├── math/           Vec2/3/4 · Mat4 · Quat · Color
│   └── util/           Timer · UUID · Filesystem · Profiler
│
├── src/
│   ├── core/           Application · Logger · Config (implementations)
│   ├── platform/
│   │   ├── windows/    Win32Window · Win32Input
│   │   ├── linux/      X11Window · X11Input
│   │   └── macos/      NSWindow · NSInput (Objective-C++)
│   ├── backend/
│   │   ├── vulkan/     vk_instance · vk_device · vk_swapchain · …
│   │   └── metal/      mtl_device · mtl_swapchain · …
│   ├── graphics/       Buffer · Mesh · Material · Camera …
│   ├── scene/          Scene · Node · Transform …
│   ├── ui/             Canvas · Widget · Button …
│   ├── math/           Mat4 · Quat …
│   └── util/           Timer · UUID · Filesystem · Profiler
│
├── shaders/            GLSL source (compile with glslc → .spv)
├── examples/           hello_window · ui_demo · hello_triangle · …
├── CMakeLists.txt
└── vcpkg.json          Zero-dependency manifest
```

---

## vcpkg Integration (consuming the library)

Add to your `vcpkg.json`:

```json
{
  "dependencies": [ "vgui" ]
}
```

Then in `CMakeLists.txt`:

```cmake
find_package(vgui CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE vgui::vgui)
```

---

## License

MIT — see `LICENSE`.
