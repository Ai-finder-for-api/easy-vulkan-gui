// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Win32Window  –  implementation                         ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "win32_window.hpp"
#if defined(_WIN32)
#include <vgui/input/event_queue.hpp>
#include <vgui/input/keyboard.hpp>
#include <vgui/input/mouse.hpp>
#include <vgui/core/logger.hpp>
#include <stdexcept>

namespace vgui {

static Key vk_to_key(WPARAM vk) {
    if (vk >= 'A' && vk <= 'Z') return static_cast<Key>(static_cast<int>(Key::A) + vk - 'A');
    if (vk >= '0' && vk <= '9') return static_cast<Key>(static_cast<int>(Key::D0) + vk - '0');
    switch (vk) {
        case VK_SPACE:   return Key::Space;
        case VK_ESCAPE:  return Key::Escape;
        case VK_RETURN:  return Key::Enter;
        case VK_TAB:     return Key::Tab;
        case VK_BACK:    return Key::Backspace;
        case VK_DELETE:  return Key::Delete;
        case VK_INSERT:  return Key::Insert;
        case VK_LEFT:    return Key::Left;
        case VK_RIGHT:   return Key::Right;
        case VK_UP:      return Key::Up;
        case VK_DOWN:    return Key::Down;
        case VK_SHIFT:   return Key::LeftShift;
        case VK_CONTROL: return Key::LeftCtrl;
        case VK_MENU:    return Key::LeftAlt;
        default:         return Key::Unknown;
    }
}

static KeyMod current_mods() {
    KeyMod m = KeyMod::None;
    if (GetKeyState(VK_SHIFT)   & 0x8000) m = m | KeyMod::Shift;
    if (GetKeyState(VK_CONTROL) & 0x8000) m = m | KeyMod::Ctrl;
    if (GetKeyState(VK_MENU)    & 0x8000) m = m | KeyMod::Alt;
    return m;
}

LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto* self = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_CLOSE:
            if (self) self->m_open = false;
            EventQueue::get().push(EvClose{});
            return 0;

        case WM_SIZE:
            if (self) { self->m_w = LOWORD(lp); self->m_h = HIWORD(lp); }
            EventQueue::get().push(EvResize{ LOWORD(lp), HIWORD(lp) });
            return 0;

        case WM_SETFOCUS:
            EventQueue::get().push(EvFocus{ true });  return 0;
        case WM_KILLFOCUS:
            EventQueue::get().push(EvFocus{ false }); return 0;

        case WM_KEYDOWN: case WM_SYSKEYDOWN:
            EventQueue::get().push(EvKeyDown{ vk_to_key(wp), static_cast<int>(wp), current_mods() });
            return 0;
        case WM_KEYUP: case WM_SYSKEYUP:
            EventQueue::get().push(EvKeyUp{ vk_to_key(wp), static_cast<int>(wp), current_mods() });
            return 0;
        case WM_CHAR:
            EventQueue::get().push(EvChar{ static_cast<uint32_t>(wp) });
            return 0;

        case WM_MOUSEMOVE:
            EventQueue::get().push(EvMouseMove{{ static_cast<float>(GET_X_LPARAM(lp)),
                                                  static_cast<float>(GET_Y_LPARAM(lp)) }});
            return 0;
        case WM_LBUTTONDOWN: EventQueue::get().push(EvMouseDown{MouseButton::Left,   {}}); return 0;
        case WM_LBUTTONUP:   EventQueue::get().push(EvMouseUp  {MouseButton::Left,   {}}); return 0;
        case WM_RBUTTONDOWN: EventQueue::get().push(EvMouseDown{MouseButton::Right,  {}}); return 0;
        case WM_RBUTTONUP:   EventQueue::get().push(EvMouseUp  {MouseButton::Right,  {}}); return 0;
        case WM_MBUTTONDOWN: EventQueue::get().push(EvMouseDown{MouseButton::Middle, {}}); return 0;
        case WM_MBUTTONUP:   EventQueue::get().push(EvMouseUp  {MouseButton::Middle, {}}); return 0;
        case WM_MOUSEWHEEL:
            EventQueue::get().push(EvScroll{{ 0, GET_WHEEL_DELTA_WPARAM(wp) / 120.f }});
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void Win32Window::create(const std::string& title, int w, int h) {
    m_w = w; m_h = h;
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"vgui_window";
    RegisterClassExW(&wc);

    int len = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
    std::wstring wtitle(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, wtitle.data(), len);

    RECT r{ 0, 0, w, h };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowExW(0, L"vgui_window", wtitle.c_str(),
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              r.right - r.left, r.bottom - r.top,
                              nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!m_hwnd) throw std::runtime_error("Win32: CreateWindow failed");

    SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    m_open = true;
    VGUI_INFO("Win32 window created");
}

void Win32Window::show()    { ShowWindow(m_hwnd, SW_SHOW); UpdateWindow(m_hwnd); }
void Win32Window::hide()    { ShowWindow(m_hwnd, SW_HIDE); }
void Win32Window::close()   { m_open = false; DestroyWindow(m_hwnd); }
void Win32Window::destroy() { if (m_hwnd) { DestroyWindow(m_hwnd); m_hwnd = nullptr; } }

void Win32Window::poll() {
    MSG msg{};
    while (PeekMessageW(&msg, m_hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void Win32Window::present() { /* swap chain present handled by GPU backend */ }

void Win32Window::set_title(const std::string& t) {
    int n = MultiByteToWideChar(CP_UTF8, 0, t.c_str(), -1, nullptr, 0);
    std::wstring w(n, 0); MultiByteToWideChar(CP_UTF8, 0, t.c_str(), -1, w.data(), n);
    SetWindowTextW(m_hwnd, w.c_str());
}

void Win32Window::set_size(int w, int h) {
    RECT r{0,0,w,h}; AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
    SetWindowPos(m_hwnd, nullptr, 0, 0, r.right-r.left, r.bottom-r.top,
                 SWP_NOMOVE | SWP_NOZORDER);
    m_w = w; m_h = h;
}

void Win32Window::set_position(int x, int y) {
    SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void Win32Window::set_resizable(bool r) {
    LONG style = GetWindowLong(m_hwnd, GWL_STYLE);
    if (r) style |=  (WS_SIZEBOX | WS_MAXIMIZEBOX);
    else   style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
    SetWindowLong(m_hwnd, GWL_STYLE, style);
}

void Win32Window::set_fullscreen(bool f) {
    static WINDOWPLACEMENT wp{sizeof(wp)};
    if (f) {
        GetWindowPlacement(m_hwnd, &wp);
        SetWindowLong(m_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
        HMONITOR hm = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi{sizeof(mi)}; GetMonitorInfo(hm, &mi);
        SetWindowPos(m_hwnd, HWND_TOP,
                     mi.rcMonitor.left, mi.rcMonitor.top,
                     mi.rcMonitor.right-mi.rcMonitor.left,
                     mi.rcMonitor.bottom-mi.rcMonitor.top,
                     SWP_FRAMECHANGED);
    } else {
        SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
        SetWindowPlacement(m_hwnd, &wp);
        SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
    }
}

void Win32Window::set_opacity(float o) { SetLayeredWindowAttributes(m_hwnd, 0, static_cast<BYTE>(o*255), LWA_ALPHA); }
void Win32Window::set_vsync(bool v)    { m_vsync = v; /* swap interval set in GPU backend */ }

int   Win32Window::width()        const { return m_w; }
int   Win32Window::height()       const { return m_h; }
Vec2  Win32Window::position()     const { RECT r; GetWindowRect(m_hwnd, &r); return {static_cast<float>(r.left), static_cast<float>(r.top)}; }
bool  Win32Window::is_open()      const { return m_open; }
bool  Win32Window::is_focused()   const { return GetForegroundWindow() == m_hwnd; }
bool  Win32Window::is_minimized() const { return IsIconic(m_hwnd) != 0; }
bool  Win32Window::is_fullscreen()const {
    DWORD style = GetWindowLong(m_hwnd, GWL_STYLE);
    return (style & WS_POPUP) != 0;
}

} // namespace vgui
#endif
