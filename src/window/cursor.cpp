// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Cursor  –  implementation                              ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/window/cursor.hpp>

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__APPLE__)
#  include <objc/runtime.h>
#  include <objc/message.h>
#elif defined(__linux__)
#  include <X11/Xlib.h>
#  include <X11/cursorfont.h>
#endif

namespace vgui {

struct Cursor::Impl {
    CursorShape shape = CursorShape::Arrow;
#if defined(_WIN32)
    HCURSOR     handle = nullptr;
#endif
};

Cursor& Cursor::get() { static Cursor s; return s; }

Cursor::Cursor() : m_impl(std::make_unique<Impl>()) {}

void Cursor::set_shape(CursorShape shape) {
    m_impl->shape = shape;
#if defined(_WIN32)
    LPCTSTR id = IDC_ARROW;
    switch (shape) {
        case CursorShape::IBeam:     id = IDC_IBEAM;     break;
        case CursorShape::Crosshair: id = IDC_CROSS;     break;
        case CursorShape::Hand:      id = IDC_HAND;      break;
        case CursorShape::ResizeH:   id = IDC_SIZEWE;    break;
        case CursorShape::ResizeV:   id = IDC_SIZENS;    break;
        case CursorShape::ResizeNWSE:id = IDC_SIZENWSE;  break;
        case CursorShape::ResizeNESW:id = IDC_SIZENESW;  break;
        case CursorShape::ResizeAll: id = IDC_SIZEALL;   break;
        case CursorShape::NotAllowed:id = IDC_NO;        break;
        case CursorShape::Wait:      id = IDC_WAIT;      break;
        case CursorShape::Hidden:    SetCursor(nullptr); return;
        default: break;
    }
    m_impl->handle = LoadCursor(nullptr, id);
    SetCursor(m_impl->handle);
#endif
}

void Cursor::set_custom(const uint8_t*, int, int, int, int) { /* platform-specific */ }
CursorShape Cursor::current_shape() const { return m_impl->shape; }
void Cursor::reset() { set_shape(CursorShape::Arrow); }

} // namespace vgui
