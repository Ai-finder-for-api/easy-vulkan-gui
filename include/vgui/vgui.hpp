#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║                         vgui  0.1.0                            ║
// ║  Zero-dependency cross-platform GPU-accelerated GUI library.    ║
// ║                                                                 ║
// ║  Backends:  Vulkan 1.3 (Windows · Linux) │ Metal 3 (macOS)     ║
// ║  Window:    Custom Win32 / X11+Wayland / NSWindow               ║
// ║  Input:     Custom keyboard · mouse · gamepad · touch           ║
// ║                                                                 ║
// ║  Quick start:                                                   ║
// ║    #include <vgui/vgui.hpp>                                     ║
// ║    int main() {                                                 ║
// ║        auto& win   = vgui::make_screen("My App", 1280, 720);    ║
// ║        auto& btn   = vgui::make_button("Hello");                ║
// ║        btn.x = 40; btn.y = 40;                                  ║
// ║        btn.on_click = []{ vgui::quit(); };                      ║
// ║        vgui::run();                                             ║
// ║    }                                                            ║
// ╚══════════════════════════════════════════════════════════════════╝

// ── Core ──────────────────────────────────────────────────────────
#include "core/version.hpp"
#include "core/config.hpp"
#include "core/logger.hpp"
#include "core/context.hpp"
#include "core/window.hpp"
#include "core/renderer.hpp"
#include "core/application.hpp"

// ── Math ──────────────────────────────────────────────────────────
#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/mat4.hpp"
#include "math/quat.hpp"
#include "math/color.hpp"

// ── Input ─────────────────────────────────────────────────────────
#include "input/keyboard.hpp"
#include "input/mouse.hpp"
#include "input/gamepad.hpp"
#include "input/touch.hpp"
#include "input/event_queue.hpp"
#include "input/input_manager.hpp"

// ── Window ────────────────────────────────────────────────────────
#include "window/window_base.hpp"
#include "window/window_manager.hpp"
#include "window/monitor.hpp"
#include "window/cursor.hpp"
#include "window/clipboard.hpp"

// ── Graphics ──────────────────────────────────────────────────────
#include "graphics/vertex.hpp"
#include "graphics/buffer.hpp"
#include "graphics/image.hpp"
#include "graphics/texture.hpp"
#include "graphics/sampler.hpp"
#include "graphics/shader.hpp"
#include "graphics/uniform.hpp"
#include "graphics/pipeline.hpp"
#include "graphics/descriptor.hpp"
#include "graphics/renderpass.hpp"
#include "graphics/framebuffer.hpp"
#include "graphics/command.hpp"
#include "graphics/mesh.hpp"
#include "graphics/material.hpp"
#include "graphics/camera.hpp"

// ── Scene ─────────────────────────────────────────────────────────
#include "scene/transform.hpp"
#include "scene/node.hpp"
#include "scene/entity.hpp"
#include "scene/scene.hpp"
#include "scene/light.hpp"
#include "scene/model.hpp"

// ── UI ────────────────────────────────────────────────────────────
#include "ui/font.hpp"
#include "ui/widget.hpp"
#include "ui/label.hpp"
#include "ui/button.hpp"
#include "ui/textbox.hpp"
#include "ui/panel.hpp"
#include "ui/layout.hpp"
#include "ui/canvas.hpp"

// ── Utilities ─────────────────────────────────────────────────────
#include "util/timer.hpp"
#include "util/uuid.hpp"
#include "util/filesystem.hpp"
#include "util/profiler.hpp"

// ╔══════════════════════════════════════════════════════════════════╗
// ║  Easy API  –  vgui.width = 1080 style                          ║
// ╚══════════════════════════════════════════════════════════════════╝
namespace vgui {

/// Create and show a window. Returns a stable reference.
/// win.width = 1920; win.height = 1080; win.title = "Hi";
inline Window& make_screen(const std::string& title  = "vgui App",
                             int               width  = 1280,
                             int               height = 720)
{
    return Application::get().make_window(title, width, height);
}

/// One-line widget factories – widgets are added to the active canvas.
inline Button&  make_button (const std::string& text = "Button")  { return Canvas::active().make<Button>(text);   }
inline Label&   make_label  (const std::string& text = "Label")   { return Canvas::active().make<Label>(text);    }
inline Textbox& make_textbox(const std::string& hint = "")        { return Canvas::active().make<Textbox>(hint);  }
inline Panel&   make_panel  ()                                     { return Canvas::active().make<Panel>();        }
inline Layout&  make_layout (LayoutType t = LayoutType::VStack)   { return Canvas::active().make<Layout>(t);      }

/// Start the main event loop (blocks until all windows are closed or quit() is called)
inline void run () { Application::get().run();  }

/// Schedule a clean exit from inside any callback or on_update
inline void quit() { Application::get().quit(); }

} // namespace vgui
