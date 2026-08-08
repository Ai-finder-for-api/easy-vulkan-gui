#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <optional>
#include "vgui/core/config.hpp"
#include "vgui/core/logger.hpp"

#ifdef VGUI_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(VGUI_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(VGUI_PLATFORM_MACOS)
    #if defined(VGUI_USE_METAL)
        // Metal backend
    #else
        #define VK_USE_PLATFORM_MACOS_MOLTENVK_KHR
    #endif
#endif

namespace vgui {

// Forward declarations
class Window;
class Renderer;
class Scene;

/**
 * @brief Main VGUI Context - manages all subsystems
 * 
 * The Context is the central manager for VGUI. It owns and coordinates
 * all major subsystems including the window, renderer, and scene.
 */
class Context {
public:
    /**
     * @brief Get the singleton context instance
     */
    static Context& getInstance() {
        static Context instance;
        return instance;
    }
    
    /**
     * @brief Initialize the VGUI context
     * @param config Configuration for initialization
     * @return true on success, false on failure
     */
    bool initialize(const Config& config);
    
    /**
     * @brief Shutdown the VGUI context
     */
    void shutdown();
    
    /**
     * @brief Check if the context is initialized
     */
    bool isInitialized() const { return initialized_; }
    
    /**
     * @brief Run the main application loop
     * @param app Application instance to run
     * @return Exit code
     */
    int run(std::shared_ptr<class Application> app);
    
    /**
     * @brief Process one frame
     */
    void processFrame();
    
    /**
     * @brief Get the main window
     */
    Window& getWindow();
    
    /**
     * @brief Get the renderer
     */
    Renderer& getRenderer();
    
    /**
     * @brief Get the active scene
     */
    Scene& getScene();
    
    /**
     * @brief Set the active scene
     */
    void setScene(std::shared_ptr<Scene> scene);
    
    /**
     * @brief Get the configuration
     */
    const Config& getConfig() const { return config_; }
    
    /**
     * @brief Check if the window should close
     */
    bool shouldClose() const;
    
    /**
     * @brief Request window close
     */
    void requestClose() { shouldClose_ = true; }
    
    /**
     * @brief Get the frame count
     */
    uint64_t getFrameCount() const { return frameCount_; }
    
    /**
     * @brief Get the current frame index
     */
    uint32_t getCurrentFrame() const { return currentFrame_; }
    
private:
    Context() = default;
    ~Context();
    
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    
    bool initializeWindow();
    bool initializeRenderer();
    void cleanup();
    
    Config config_;
    std::atomic<bool> initialized_{false};
    std::atomic<bool> shouldClose_{false};
    
    std::shared_ptr<Window> window_;
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<Scene> scene_;
    
    std::shared_ptr<Application> application_;
    
    uint64_t frameCount_ = 0;
    uint32_t currentFrame_ = 0;
    
    std::mutex stateMutex_;
};

} // namespace vgui
