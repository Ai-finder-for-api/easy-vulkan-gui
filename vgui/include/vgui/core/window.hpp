#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <memory>
#include "vgui/core/config.hpp"

namespace vgui {

// Forward declaration
class Context;

/**
 * @brief Window class - wraps the platform window
 * 
 * Provides a cross-platform window with keyboard, mouse, and 
 * scroll input handling. Uses GLFW internally.
 */
class Window {
public:
    using ResizeCallback = std::function<void(uint32_t, uint32_t)>;
    using KeyCallback = std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)>;
    using MouseCallback = std::function<void(uint32_t, uint32_t, uint32_t)>;
    using ScrollCallback = std::function<void(double, double)>;
    using CharCallback = std::function<void(uint32_t)>;
    
    /**
     * @brief Constructor
     */
    Window();
    
    /**
     * @brief Destructor
     */
    ~Window();
    
    /**
     * @brief Initialize the window
     * @param hints Window creation hints
     * @return true on success
     */
    bool init(const WindowHints& hints);
    
    /**
     * @brief Shutdown the window
     */
    void shutdown();
    
    /**
     * @brief Check if window should close
     */
    bool shouldClose() const;
    
    /**
     * @brief Close the window
     */
    void close();
    
    /**
     * @brief Swap buffers (call at end of frame)
     */
    void swapBuffers();
    
    /**
     * @brief Poll for events
     */
    void pollEvents();
    
    /**
     * @brief Get window width
     */
    uint32_t getWidth() const { return width_; }
    
    /**
     * @brief Get window height
     */
    uint32_t getHeight() const { return height_; }
    
    /**
     * @brief Get window size
     */
    void getSize(uint32_t& width, uint32_t& height) const {
        width = width_;
        height = height_;
    }
    
    /**
     * @brief Set window size
     */
    void setSize(uint32_t width, uint32_t height);
    
    /**
     * @brief Get framebuffer size (may differ from window size on high-DPI)
     */
    void getFramebufferSize(uint32_t& width, uint32_t& height) const {
        width = framebufferWidth_;
        height = framebufferHeight_;
    }
    
    /**
     * @brief Check if window is minimized
     */
    bool isMinimized() const { return minimized_; }
    
    /**
     * @brief Check if window is maximized
     */
    bool isMaximized() const { return maximized_; }
    
    /**
     * @brief Get the native window handle (platform-specific)
     */
    void* getNativeHandle() const { return nativeHandle_; }
    
    /**
     * @brief Get window title
     */
    std::string getTitle() const;
    
    /**
     * @brief Set window title
     */
    void setTitle(const std::string& title);
    
    /**
     * @brief Set window position
     */
    void setPosition(int32_t x, int32_t y);
    
    /**
     * @brief Get cursor position
     */
    void getCursorPos(double& x, double& y) const;
    
    /**
     * @brief Set cursor position
     */
    void setCursorPos(double x, double y);
    
    /**
     * @brief Show/hide cursor
     */
    void showCursor(bool show);
    
    /**
     * @brief Check if cursor is visible
     */
    bool isCursorVisible() const;
    
    /**
     * @brief Set input mode
     */
    void setInputMode(int32_t mode, int32_t value);
    
    /**
     * @brief Get input mode
     */
    int32_t getInputMode(int32_t mode) const;
    
    /**
     * @brief Make context current
     */
    void makeContextCurrent();
    
    // Callback setters
    void setResizeCallback(ResizeCallback callback) { resizeCallback_ = callback; }
    void setKeyCallback(KeyCallback callback) { keyCallback_ = callback; }
    void setMouseCallback(MouseCallback callback) { mouseCallback_ = callback; }
    void setScrollCallback(ScrollCallback callback) { scrollCallback_ = callback; }
    void setCharCallback(CharCallback callback) { charCallback_ = callback; }
    
    /**
     * @brief Check if this is the current context
     */
    bool isCurrentContext() const;
    
    /**
     * @brief Get frame timing information
     */
    double getTimeSinceLastFrame() const { return lastFrameTime_; }
    
private:
    friend class Context;
    
    void* nativeHandle_ = nullptr;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t framebufferWidth_ = 0;
    uint32_t framebufferHeight_ = 0;
    bool minimized_ = false;
    bool maximized_ = false;
    
    ResizeCallback resizeCallback_;
    KeyCallback keyCallback_;
    MouseCallback mouseCallback_;
    ScrollCallback scrollCallback_;
    CharCallback charCallback_;
    
    double lastFrameTime_ = 0.0;
    double currentTime_ = 0.0;
    
    // Platform-specific members
#ifdef VGUI_PLATFORM_WINDOWS
    // Windows-specific
#elif defined(VGUI_PLATFORM_LINUX)
    // Linux-specific
#elif defined(VGUI_PLATFORM_MACOS)
    // macOS-specific
#endif
};

} // namespace vgui
