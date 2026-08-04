#pragma once
#include <vulkan/vulkan.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#elif defined(__APPLE__)
#import <Cocoa/Cocoa.h>
#elif defined(ANDROID)
#include <android/native_activity.h>
#include <android_native_app_glue.h>
#endif

namespace EasyVK {

class Window {
public:
    Window(const std::string& title, int width, int height);
    ~Window();
    void pollEvents();
    bool shouldClose();
    VkSurfaceKHR createSurface(VkInstance instance);
    
private:
    int width;
    int height;
    std::string title;
    bool closed = false;

#ifdef _WIN32
    HWND hwnd;
    HINSTANCE hinstance;
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#elif defined(__linux__)
    Display* display;
    ::Window window; 
    Atom wmDeleteWindow;
#elif defined(__APPLE__)
    NSWindow* nsWindow;
#elif defined(ANDROID)
    ANativeWindow* androidWindow;
#endif
};

}