#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Window                                                 ║
// ║  Platform-agnostic window with fluent property setters.         ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto& win  = vgui::make_screen("My App");                    ║
// ║    win.width  = 1920;                                           ║
// ║    win.height = 1080;                                           ║
// ║    win.title  = "Hello!";                                       ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string>
#include <functional>
#include <memory>
#include <cstdint>

namespace vgui {

class Window {
public:
    // ── Fluent properties ─────────────────────────────────
    struct Props {
        std::string title      = "vgui";
        int         width      = 1280;
        int         height     = 720;
        bool        resizable  = true;
        bool        vsync      = true;
        bool        fullscreen = false;
        float       opacity    = 1.0f;
    } props;

    // Direct-assign syntax:  win.width = 1920
    // (Proxy setters keep the GPU surface in sync.)
    void set_title     (const std::string& t);
    void set_size      (int w, int h);
    void set_fullscreen(bool f);
    void set_vsync     (bool v);
    void set_opacity   (float o);
    void set_resizable (bool r);

    // ── Queries ───────────────────────────────────────────
    int         width()      const;
    int         height()     const;
    float       aspect()     const;   ///< width / height
    bool        is_open()    const;
    bool        is_focused() const;
    bool        is_minimized() const;
    void*       native_handle() const; ///< HWND / NSWindow* / Window (X11)

    // ── Lifecycle callbacks ───────────────────────────────
    std::function<void(int w, int h)> on_resize;
    std::function<void()>             on_close;
    std::function<void(bool focused)> on_focus;

    // ── Internal ──────────────────────────────────────────
    void poll_events();
    void present();
    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    friend class Application;
    Window() = default;
};

} // namespace vgui
