// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: X11Window  –  implementation                           ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "x11_window.hpp"
#if defined(__linux__)
#include <vgui/input/event_queue.hpp>
#include <vgui/core/logger.hpp>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdexcept>
#include <cstring>

namespace vgui {

static Key keysym_to_key(KeySym ks) {
    if (ks >= XK_a && ks <= XK_z) return static_cast<Key>(static_cast<int>(Key::A) + ks - XK_a);
    if (ks >= XK_0 && ks <= XK_9) return static_cast<Key>(static_cast<int>(Key::D0) + ks - XK_0);
    switch (ks) {
        case XK_space:   return Key::Space;
        case XK_Escape:  return Key::Escape;
        case XK_Return:  return Key::Enter;
        case XK_Tab:     return Key::Tab;
        case XK_BackSpace:return Key::Backspace;
        case XK_Delete:  return Key::Delete;
        case XK_Insert:  return Key::Insert;
        case XK_Left:    return Key::Left;
        case XK_Right:   return Key::Right;
        case XK_Up:      return Key::Up;
        case XK_Down:    return Key::Down;
        case XK_Shift_L: return Key::LeftShift;
        case XK_Control_L:return Key::LeftCtrl;
        case XK_Alt_L:   return Key::LeftAlt;
        default:         return Key::Unknown;
    }
}

void X11Window::create(const std::string& title, int w, int h) {
    m_w = w; m_h = h;
    m_dpy = XOpenDisplay(nullptr);
    if (!m_dpy) throw std::runtime_error("X11: Cannot open display");

    int scr = DefaultScreen(m_dpy);
    m_win   = XCreateSimpleWindow(m_dpy, RootWindow(m_dpy, scr),
                                  0, 0, w, h, 0,
                                  BlackPixel(m_dpy, scr),
                                  WhitePixel(m_dpy, scr));

    XSelectInput(m_dpy, m_win,
                 ExposureMask | KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask |
                 PointerMotionMask | StructureNotifyMask | FocusChangeMask);

    m_wm_delete = XInternAtom(m_dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_dpy, m_win, &m_wm_delete, 1);
    XStoreName(m_dpy, m_win, title.c_str());
    m_open = true;
    VGUI_INFO("X11 window created");
}

void X11Window::show()    { XMapWindow(m_dpy, m_win); XFlush(m_dpy); }
void X11Window::hide()    { XUnmapWindow(m_dpy, m_win); }
void X11Window::close()   { m_open = false; }
void X11Window::destroy() { if (m_win) { XDestroyWindow(m_dpy, m_win); m_win = 0; } if (m_dpy) { XCloseDisplay(m_dpy); m_dpy = nullptr; } }
void X11Window::present() {}

void X11Window::poll() {
    XEvent xe;
    while (XPending(m_dpy)) {
        XNextEvent(m_dpy, &xe);
        switch (xe.type) {
            case ClientMessage:
                if (static_cast<Atom>(xe.xclient.data.l[0]) == m_wm_delete) {
                    m_open = false;
                    EventQueue::get().push(EvClose{});
                }
                break;
            case ConfigureNotify:
                m_w = xe.xconfigure.width;
                m_h = xe.xconfigure.height;
                EventQueue::get().push(EvResize{m_w, m_h});
                break;
            case FocusIn:  m_focused = true;  EventQueue::get().push(EvFocus{true});  break;
            case FocusOut: m_focused = false; EventQueue::get().push(EvFocus{false}); break;
            case KeyPress: {
                KeySym ks = XLookupKeysym(&xe.xkey, 0);
                EventQueue::get().push(EvKeyDown{keysym_to_key(ks), static_cast<int>(ks), KeyMod::None});
                // Emit char
                char buf[8]{}; int n = XLookupString(&xe.xkey, buf, sizeof(buf), nullptr, nullptr);
                if (n > 0) EventQueue::get().push(EvChar{static_cast<uint32_t>(buf[0])});
                break;
            }
            case KeyRelease: {
                KeySym ks = XLookupKeysym(&xe.xkey, 0);
                EventQueue::get().push(EvKeyUp{keysym_to_key(ks), static_cast<int>(ks), KeyMod::None});
                break;
            }
            case ButtonPress: {
                MouseButton btn = xe.xbutton.button == 1 ? MouseButton::Left :
                                  xe.xbutton.button == 2 ? MouseButton::Middle : MouseButton::Right;
                if (xe.xbutton.button == 4) EventQueue::get().push(EvScroll{{ 0,  1}});
                else if (xe.xbutton.button == 5) EventQueue::get().push(EvScroll{{ 0, -1}});
                else EventQueue::get().push(EvMouseDown{btn, {}});
                break;
            }
            case ButtonRelease: {
                if (xe.xbutton.button >= 4) break;
                MouseButton btn = xe.xbutton.button == 1 ? MouseButton::Left :
                                  xe.xbutton.button == 2 ? MouseButton::Middle : MouseButton::Right;
                EventQueue::get().push(EvMouseUp{btn, {}});
                break;
            }
            case MotionNotify:
                EventQueue::get().push(EvMouseMove{{static_cast<float>(xe.xmotion.x),
                                                     static_cast<float>(xe.xmotion.y)}});
                break;
        }
    }
}

void X11Window::set_title(const std::string& t) { XStoreName(m_dpy, m_win, t.c_str()); }
void X11Window::set_size (int w, int h)          { XResizeWindow(m_dpy, m_win, w, h); m_w=w; m_h=h; }
void X11Window::set_position(int x, int y)       { XMoveWindow(m_dpy, m_win, x, y); }
void X11Window::set_resizable(bool r) {
    XSizeHints hints{}; hints.flags = PMinSize | PMaxSize;
    hints.min_width  = r ? 0     : m_w; hints.max_width  = r ? 65535 : m_w;
    hints.min_height = r ? 0     : m_h; hints.max_height = r ? 65535 : m_h;
    XSetWMNormalHints(m_dpy, m_win, &hints);
}
void X11Window::set_fullscreen(bool f) {
    m_fullscreen = f;
    XEvent xe{}; xe.type = ClientMessage;
    xe.xclient.window  = m_win;
    xe.xclient.message_type = XInternAtom(m_dpy, "_NET_WM_STATE", False);
    xe.xclient.format  = 32;
    xe.xclient.data.l[0] = f ? 1 : 0;
    xe.xclient.data.l[1] = XInternAtom(m_dpy, "_NET_WM_STATE_FULLSCREEN", False);
    XSendEvent(m_dpy, DefaultRootWindow(m_dpy), False,
               SubstructureNotifyMask | SubstructureRedirectMask, &xe);
}
void X11Window::set_opacity(float o) {
    uint32_t val = static_cast<uint32_t>(o * 0xFFFFFFFFu);
    Atom atom = XInternAtom(m_dpy, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(m_dpy, m_win, atom, XA_CARDINAL, 32, PropModeReplace,
                    reinterpret_cast<unsigned char*>(&val), 1);
}
void X11Window::set_vsync(bool) {}

} // namespace vgui
#endif
