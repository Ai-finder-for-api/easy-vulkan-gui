#include "vgui/core/window.hpp"

#ifdef VGUI_PLATFORM_WINDOWS
    #include <GLFW/glfw3.h>
#elif defined(VGUI_PLATFORM_LINUX)
    #include <GLFW/glfw3.h>
#elif defined(VGUI_PLATFORM_MACOS)
    #include <GLFW/glfw3.h>
#endif

#include "vgui/util/logger.hpp"

namespace vgui {

Window::Window() {
    VGUI_LOG_DEBUG("Window", "Window created");
}

Window::~Window() {
    shutdown();
}

bool Window::init(const WindowHints& hints) {
    VGUI_LOG_INFO("Window", "Initializing window: " 
                  << hints.title << " " 
                  << hints.width << "x" << hints.height);
    
    // Initialize GLFW if needed
    if (!glfwInit()) {
        VGUI_LOG_ERROR("Window", "Failed to initialize GLFW");
        return false;
    }
    
    // Set window hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // Vulkan doesn't need OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, hints.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    
    if (hints.fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        nativeHandle_ = glfwCreateWindow(mode->width, mode->height, 
                                         hints.title.c_str(), monitor, nullptr);
    } else {
        nativeHandle_ = glfwCreateWindow(hints.width, hints.height, 
                                         hints.title.c_str(), nullptr, nullptr);
    }
    
    if (!nativeHandle_) {
        VGUI_LOG_ERROR("Window", "Failed to create window");
        glfwTerminate();
        return false;
    }
    
    // Get actual framebuffer size
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(nativeHandle_), &fbWidth, &fbHeight);
    framebufferWidth_ = fbWidth;
    framebufferHeight_ = fbHeight;
    
    width_ = hints.width;
    height_ = hints.height;
    
    VGUI_LOG_INFO("Window", "Window created successfully: " 
                  << width_ << "x" << height_ 
                  << " (framebuffer: " << framebufferWidth_ << "x" << framebufferHeight_ << ")");
    
    return true;
}

void Window::shutdown() {
    if (nativeHandle_) {
        glfwDestroyWindow(static_cast<GLFWwindow*>(nativeHandle_));
        nativeHandle_ = nullptr;
    }
    
    glfwTerminate();
    VGUI_LOG_INFO("Window", "Window shutdown complete");
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(static_cast<GLFWwindow*>(nativeHandle_));
}

void Window::close() {
    glfwSetWindowShouldClose(static_cast<GLFWwindow*>(nativeHandle_), GLFW_TRUE);
}

void Window::swapBuffers() {
    glfwSwapBuffers(static_cast<GLFWwindow*>(nativeHandle_));
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::setSize(uint32_t width, uint32_t height) {
    glfwSetWindowSize(static_cast<GLFWwindow*>(nativeHandle_), 
                     static_cast<int>(width), static_cast<int>(height));
    width_ = width;
    height_ = height;
}

void Window::setPosition(int32_t x, int32_t y) {
    glfwSetWindowPos(static_cast<GLFWwindow*>(nativeHandle_), x, y);
}

void Window::getCursorPos(double& x, double& y) const {
    glfwGetCursorPos(static_cast<GLFWwindow*>(nativeHandle_), &x, &y);
}

void Window::setCursorPos(double x, double y) {
    glfwSetCursorPos(static_cast<GLFWwindow*>(nativeHandle_), x, y);
}

void Window::showCursor(bool show) {
    glfwSetInputMode(static_cast<GLFWwindow*>(nativeHandle_), 
                    GLFW_CURSOR, 
                    show ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

bool Window::isCursorVisible() const {
    return glfwGetInputMode(static_cast<GLFWwindow*>(nativeHandle_), 
                           GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

void Window::setInputMode(int32_t mode, int32_t value) {
    glfwSetInputMode(static_cast<GLFWwindow*>(nativeHandle_), mode, value);
}

int32_t Window::getInputMode(int32_t mode) const {
    return glfwGetInputMode(static_cast<GLFWwindow*>(nativeHandle_), mode);
}

void Window::makeContextCurrent() {
    glfwMakeContextCurrent(static_cast<GLFWwindow*>(nativeHandle_));
}

std::string Window::getTitle() const {
    const char* title = glfwGetWindowTitle(static_cast<GLFWwindow*>(nativeHandle_));
    return title ? std::string(title) : "";
}

void Window::setTitle(const std::string& title) {
    glfwSetWindowTitle(static_cast<GLFWwindow*>(nativeHandle_), title.c_str());
}

bool Window::isCurrentContext() const {
    return glfwGetCurrentContext() == static_cast<GLFWwindow*>(nativeHandle_);
}

// Callback wrappers
static void resizeCallback(GLFWwindow* window, int width, int height) {
    // Would need to route to Window instance
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Would need to route to Window instance
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Would need to route to Window instance
}

static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // Would need to route to Window instance
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Would need to route to Window instance
}

static void charCallback(GLFWwindow* window, unsigned int codepoint) {
    // Would need to route to Window instance
}

} // namespace vgui
