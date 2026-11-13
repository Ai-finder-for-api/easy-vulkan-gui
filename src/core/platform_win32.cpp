#if defined(_WIN32)
// Qt's CMake target defines UNICODE for everything linked to it; this file uses the ANSI (...A) Win32 API names.
#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "platform.hpp"

namespace evgx { namespace plat {
namespace {
const char* kClass = "evg_window";
bool g_class = false;
double g_qpf = 0;

int mouse_button(UINT m) {
    return (m == WM_LBUTTONDOWN || m == WM_LBUTTONUP) ? 0 : (m == WM_RBUTTONDOWN || m == WM_RBUTTONUP) ? 1 : 2;
}

LRESULT CALLBACK wndproc(HWND h, UINT m, WPARAM wp, LPARAM lp) {
    if (m == WM_NCCREATE) {
        CREATESTRUCTA* cs = reinterpret_cast<CREATESTRUCTA*>(lp);
        SetWindowLongPtrA(h, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return DefWindowProcA(h, m, wp, lp);
    }
    Window* w = reinterpret_cast<Window*>(GetWindowLongPtrA(h, GWLP_USERDATA));
    if (w) {
        switch (m) {
        case WM_SIZE: if (wp != SIZE_MINIMIZED) w->on_resize(LOWORD(lp), HIWORD(lp)); return 0;
        case WM_CLOSE: w->closed = true; return 0;                       // destroyed later by destroywindow()
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps; BeginPaint(h, &ps);
            present(w);
            EndPaint(h, &ps);
            return 0;
        }
        case WM_KEYDOWN: case WM_SYSKEYDOWN: w->on_key((int)wp, true, (lp & (1 << 30)) != 0); return 0;
        case WM_KEYUP: case WM_SYSKEYUP: w->on_key((int)wp, false); return 0;
        case WM_KILLFOCUS: for (int k = 0; k < 256; k++) w->keys[k] = 0; for (int b = 0; b < 3; b++) w->buttons[b] = false; return 0;
        case WM_MOUSEMOVE: w->on_mouse_move((short)LOWORD(lp), (short)HIWORD(lp)); return 0;
        case WM_LBUTTONDOWN: case WM_RBUTTONDOWN: case WM_MBUTTONDOWN:
            SetCapture(h); w->on_button(mouse_button(m), true); return 0;
        case WM_LBUTTONUP: case WM_RBUTTONUP: case WM_MBUTTONUP:
            w->on_button(mouse_button(m), false);
            if (!w->buttons[0] && !w->buttons[1] && !w->buttons[2]) ReleaseCapture();
            return 0;
        case WM_MOUSEWHEEL: w->on_wheel((float)(short)HIWORD(wp) / 120.f); return 0;
        }
    }
    return DefWindowProcA(h, m, wp, lp);
}
}

void* load_vulkan() {
    HMODULE m = LoadLibraryA("vulkan-1.dll");
    if (!m) return nullptr;
    return reinterpret_cast<void*>(GetProcAddress(m, "vkGetInstanceProcAddr"));
}

bool window_create(Window* w, int width, int height, const char* title, const char** err) {
    HINSTANCE inst = GetModuleHandleA(nullptr);
    if (!g_class) {
        WNDCLASSA wc = {};
        wc.lpfnWndProc = wndproc; wc.hInstance = inst; wc.lpszClassName = kClass;
        wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
        if (!RegisterClassA(&wc)) { *err = "RegisterClass failed"; return false; }
        g_class = true;
    }
    RECT r = {0, 0, width, height};
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);   // make the *client* area width x height
    HWND h = CreateWindowExA(0, kClass, title ? title : "evg", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
                             nullptr, nullptr, inst, w);
    if (!h) { *err = "CreateWindow failed"; return false; }
    w->native = h;
    return true;
}

void window_destroy(Window* w) {
    if (w->native) { SetWindowLongPtrA(static_cast<HWND>(w->native), GWLP_USERDATA, 0); DestroyWindow(static_cast<HWND>(w->native)); w->native = nullptr; }
}

void present(Window* w) {
    if (!w->native || !w->color || w->fbw <= 0 || w->fbh <= 0) return;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w->fbw;
    bi.bmiHeader.biHeight = -w->fbh;                    // top-down
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    HWND h = static_cast<HWND>(w->native);
    HDC dc = GetDC(h);
    SetDIBitsToDevice(dc, 0, 0, w->fbw, w->fbh, 0, 0, 0, w->fbh, w->color, &bi, DIB_RGB_COLORS);
    ReleaseDC(h, dc);
}

void* app_instance() { return GetModuleHandleA(nullptr); }

// A host (e.g. Qt) owns the window class; its background brush would paint over our presented frames.
void prepare_external(void* native) {
    if (native) SetClassLongPtrW(static_cast<HWND>(native), GCLP_HBRBACKGROUND, 0);
}

void pump() {
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessageA(&msg); }
}

void sleep_ms(int ms) { Sleep(static_cast<DWORD>(ms)); }

double time_seconds() {
    LARGE_INTEGER c;
    if (g_qpf == 0) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); g_qpf = (double)f.QuadPart; }
    QueryPerformanceCounter(&c);
    return (double)c.QuadPart / g_qpf;
}

void* raw_alloc(size_t n) { return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, n); }
void raw_free(void* p, size_t) { if (p) HeapFree(GetProcessHeap(), 0, p); }

long long file_size(const char* path) {
    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return -1;
    LARGE_INTEGER sz; BOOL ok = GetFileSizeEx(f, &sz); CloseHandle(f);
    return ok ? (long long)sz.QuadPart : -1;
}
bool file_read(const char* path, void* dst, size_t n) {
    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    size_t got = 0;
    while (got < n) {
        DWORD chunk = (DWORD)((n - got) > 0x40000000u ? 0x40000000u : (n - got)), r = 0;
        if (!ReadFile(f, (char*)dst + got, chunk, &r, nullptr) || r == 0) break;
        got += r;
    }
    CloseHandle(f);
    return got == n;
}
bool file_write(const char* path, const void* src, size_t n) {
    HANDLE f = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    size_t put = 0;
    while (put < n) {
        DWORD chunk = (DWORD)((n - put) > 0x40000000u ? 0x40000000u : (n - put)), r = 0;
        if (!WriteFile(f, (const char*)src + put, chunk, &r, nullptr) || r == 0) break;
        put += r;
    }
    CloseHandle(f);
    return put == n;
}
}}
#endif
