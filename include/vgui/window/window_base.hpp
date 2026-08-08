#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: WindowBase                                             ║
// ║  Abstract base for Win32 / X11 / Wayland / NSWindow backends.  ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec2.hpp"
#include <string>
#include <functional>

namespace vgui {

class WindowBase {
public:
    virtual ~WindowBase() = default;

    virtual void create (const std::string& title, int w, int h) = 0;
    virtual void destroy() = 0;
    virtual void poll   () = 0;
    virtual void show   () = 0;
    virtual void hide   () = 0;
    virtual void close  () = 0;
    virtual void present() = 0;

    virtual void  set_title     (const std::string& t)   = 0;
    virtual void  set_size      (int w, int h)            = 0;
    virtual void  set_position  (int x, int y)            = 0;
    virtual void  set_resizable (bool r)                  = 0;
    virtual void  set_fullscreen(bool f)                  = 0;
    virtual void  set_opacity   (float o)                 = 0;
    virtual void  set_vsync     (bool v)                  = 0;

    virtual int   width ()          const = 0;
    virtual int   height()          const = 0;
    virtual Vec2  position()        const = 0;
    virtual bool  is_open()         const = 0;
    virtual bool  is_focused()      const = 0;
    virtual bool  is_minimized()    const = 0;
    virtual bool  is_fullscreen()   const = 0;
    virtual void* native_handle()   const = 0; ///< HWND | NSWindow* | ::Window

    // ── Surface creation helper ───────────────────────────
    /// Returns the platform-specific surface info needed by the GPU backend
    virtual void* surface_info()    const = 0;
};

} // namespace vgui
