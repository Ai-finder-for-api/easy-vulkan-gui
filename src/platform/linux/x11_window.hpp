#pragma once
#include <vgui/window/window_base.hpp>
#if defined(__linux__)
#include <X11/Xlib.h>
#include <string>
namespace vgui {
class X11Window : public WindowBase {
public:
    void create (const std::string& title, int w, int h) override;
    void destroy() override;
    void poll   () override;
    void show   () override;
    void hide   () override;
    void close  () override;
    void present() override;
    void set_title     (const std::string& t) override;
    void set_size      (int w, int h)         override;
    void set_position  (int x, int y)         override;
    void set_resizable (bool r)               override;
    void set_fullscreen(bool f)               override;
    void set_opacity   (float o)             override;
    void set_vsync     (bool v)              override;
    int   width ()         const override { return m_w; }
    int   height()         const override { return m_h; }
    Vec2  position()       const override { return m_pos; }
    bool  is_open()        const override { return m_open; }
    bool  is_focused()     const override { return m_focused; }
    bool  is_minimized()   const override { return m_minimized; }
    bool  is_fullscreen()  const override { return m_fullscreen; }
    void* native_handle()  const override { return reinterpret_cast<void*>(m_win); }
    void* surface_info()   const override { return m_dpy; }
private:
    Display* m_dpy        = nullptr;
    ::Window m_win        = 0;
    Atom     m_wm_delete  = 0;
    int      m_w = 0, m_h = 0;
    Vec2     m_pos{};
    bool     m_open       = false;
    bool     m_focused    = false;
    bool     m_minimized  = false;
    bool     m_fullscreen = false;
};
}
#endif
