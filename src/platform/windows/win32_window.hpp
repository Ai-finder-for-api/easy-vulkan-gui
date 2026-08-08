#pragma once
#include <vgui/window/window_base.hpp>
#if defined(_WIN32)
#include <windows.h>
namespace vgui {
class Win32Window : public WindowBase {
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
    int   width ()         const override;
    int   height()         const override;
    Vec2  position()       const override;
    bool  is_open()        const override;
    bool  is_focused()     const override;
    bool  is_minimized()   const override;
    bool  is_fullscreen()  const override;
    void* native_handle()  const override { return m_hwnd; }
    void* surface_info()   const override { return m_hwnd; }
private:
    static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    HWND  m_hwnd   = nullptr;
    bool  m_open   = false;
    bool  m_vsync  = true;
    int   m_w = 0, m_h = 0;
};
}
#endif
