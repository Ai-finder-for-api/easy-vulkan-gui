#include "Window.h"
#include <stdexcept>

namespace EasyVK {

// ---------------- WINDOWS ----------------
#ifdef _WIN32
LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_CLOSE:
            if (window) window->closed = true;
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

Window::Window(const std::string& title, int width, int height) : title(title), width(width), height(height) {
    hinstance = GetModuleHandle(nullptr);
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hinstance;
    wc.lpszClassName = "EasyVKWindowClass";
    RegisterClassEx(&wc);

    hwnd = CreateWindowEx(0, "EasyVKWindowClass", title.c_str(), WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hinstance, nullptr);

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    ShowWindow(hwnd, SW_SHOW);
}

Window::~Window() {
    if (hwnd) DestroyWindow(hwnd);
    UnregisterClass("EasyVKWindowClass", hinstance);
}

void Window::pollEvents() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) closed = true;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool Window::shouldClose() { return closed; }

VkSurfaceKHR Window::createSurface(VkInstance instance) {
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = hwnd;
    createInfo.hinstance = hinstance;
    VkSurfaceKHR surface;
    if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Win32 surface");
    }
    return surface;
}

// ---------------- LINUX ----------------
#elif defined(__linux__)
Window::Window(const std::string& title, int width, int height) : title(title), width(width), height(height) {
    display = XOpenDisplay(nullptr);
    if (!display) throw std::runtime_error("Failed to open X display");
    
    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, 1, BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, title.c_str());
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    
    wmDeleteWindow = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wmDeleteWindow, 1);
}

Window::~Window() {
    if (display && window) XDestroyWindow(display, window);
    if (display) XCloseDisplay(display);
}

void Window::pollEvents() {
    while (XPending(display)) {
        XEvent event;
        XNextEvent(display, &event);
        if (event.type == ClientMessage) {
            if (static_cast<Atom>(event.xclient.data.l[0]) == wmDeleteWindow) {
                closed = true;
            }
        }
    }
}

bool Window::shouldClose() { return closed; }

VkSurfaceKHR Window::createSurface(VkInstance instance) {
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = display;
    createInfo.window = window;
    VkSurfaceKHR surface;
    if (vkCreateXlibSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Xlib surface");
    }
    return surface;
}

// ---------------- macOS ----------------
#elif defined(__APPLE__)
Window::Window(const std::string& title, int width, int height) : title(title), width(width), height(height) {
    @autoreleasepool {
        NSRect frame = NSMakeRect(0, 0, width, height);
        NSUInteger styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
        nsWindow = [[NSWindow alloc] initWithContentRect:frame styleMask:styleMask backing:NSBackingStoreBuffered defer:NO];
        [nsWindow setTitle:[NSString stringWithUTF8String:title.c_str()]];
        [nsWindow makeKeyAndOrderFront:nil];
    }
}

Window::~Window() {
    @autoreleasepool {
        [nsWindow close];
    }
}

void Window::pollEvents() {
    @autoreleasepool {
        NSEvent *event;
        while ((event = [nsWindow nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES])) {
            [nsWindow sendEvent:event];
            if ([event type] == NSEventTypeKeyDown) closed = true; 
        }
    }
}

bool Window::shouldClose() { return closed; }

VkSurfaceKHR Window::createSurface(VkInstance instance) {
    VkMacOSSurfaceCreateInfoMVK createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    createInfo.pView = (__bridge void*)[nsWindow contentView];
    VkSurfaceKHR surface;
    if (vkCreateMacOSSurfaceMVK(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create macOS surface");
    }
    return surface;
}

// ---------------- ANDROID ----------------
#elif defined(ANDROID)
// On Android, the OS creates the window and passes it to us via android_app.
// We use a dummy app pointer here just for the stub.
extern struct android_app* android_app_global;

Window::Window(const std::string& title, int width, int height) : title(title), width(width), height(height) {
    if (android_app_global && android_app_global->window) {
        androidWindow = android_app_global->window;
    } else {
        throw std::runtime_error("Android window not available yet.");
    }
}

Window::~Window() { androidWindow = nullptr; }

void Window::pollEvents() { /* Event polling on Android is handled by the main app loop */ }
bool Window::shouldClose() { return android_app_global->destroyRequested != 0; }

VkSurfaceKHR Window::createSurface(VkInstance instance) {
    VkAndroidSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    createInfo.window = androidWindow;
    VkSurfaceKHR surface;
    if (vkCreateAndroidSurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Android surface");
    }
    return surface;
}
#endif

}