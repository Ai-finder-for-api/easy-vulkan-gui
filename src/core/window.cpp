// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Window (public handle)  –  implementation stub         ║
// ╚══════════════════════════════════════════════════════════════════╝
#include <vgui/core/window.hpp>
namespace vgui {
// Public Window is a thin façade; real work in the platform WindowBase.
void Window::set_title     (const std::string&) {}
void Window::set_size      (int, int)           {}
void Window::set_fullscreen(bool)               {}
void Window::set_vsync     (bool)               {}
void Window::set_opacity   (float)              {}
void Window::set_resizable (bool)               {}
int  Window::width()        const { return props.width;  }
int  Window::height()       const { return props.height; }
float Window::aspect()      const { return props.height ? (float)props.width/props.height : 1.f; }
bool Window::is_open()      const { return true; }
bool Window::is_focused()   const { return true; }
bool Window::is_minimized() const { return false; }
void* Window::native_handle()const{ return nullptr; }
void Window::poll_events()  {}
void Window::present()      {}
void Window::close()        {}
} // namespace vgui
