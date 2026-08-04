#include "Window.h"

#ifdef _WIN32
namespace EasyVK {

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
        if (msg.message == WM_QUIT) {
            closed = true;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool Window::shouldClose() {
    return closed;
}

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

}
#endif