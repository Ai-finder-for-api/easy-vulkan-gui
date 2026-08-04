#pragma once
#include <vulkan/vulkan.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
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
#endif
};

}