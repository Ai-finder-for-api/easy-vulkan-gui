#include "vgui/core/context.hpp"
#include "vgui/core/window.hpp"
#include "vgui/core/renderer.hpp"
#include "vgui/scene/scene.hpp"
#include "vgui/ui/canvas.hpp"
#include "vgui/util/timer.hpp"
#include "vgui/util/logger.hpp"

#ifdef VGUI_PLATFORM_WINDOWS
    #include <vulkan/vulkan.hpp>
#elif defined(VGUI_PLATFORM_LINUX)
    #include <vulkan/vulkan.h>
#endif

namespace vgui {

Context::~Context() {
    shutdown();
}

bool Context::initialize(const Config& config) {
    if (initialized_) {
        VGUI_LOG_WARN("Context", "Already initialized");
        return true;
    }
    
    VGUI_LOG_INFO("Context", "Initializing VGUI v" << VGUI_VERSION.toString());
    
    // Store configuration
    config_ = config;
    
    // Initialize logging
    Logger::getInstance().init(
        config.enableValidation ? LogLevel::Debug : LogLevel::Info,
        config.enableValidation ? "vgui.log" : std::nullopt
    );
    
    VGUI_LOG_INFO("Context", "Logger initialized");
    
    // Initialize window
    if (!initializeWindow()) {
        VGUI_LOG_ERROR("Context", "Failed to initialize window");
        return false;
    }
    
    VGUI_LOG_INFO("Context", "Window created: " 
                  << config.window.width << "x" << config.window.height);
    
    // Initialize renderer
    if (!initializeRenderer()) {
        VGUI_LOG_ERROR("Context", "Failed to initialize renderer");
        return false;
    }
    
    VGUI_LOG_INFO("Context", "Renderer initialized");
    
    // Create default scene
    scene_ = Scene::create();
    
    // Create UI canvas
    auto canvas = Canvas::create(config.window.width, config.window.height);
    canvas->setName("MainCanvas");
    
    initialized_ = true;
    VGUI_LOG_INFO("Context", "VGUI initialized successfully");
    
    return true;
}

void Context::shutdown() {
    if (!initialized_) return;
    
    VGUI_LOG_INFO("Context", "Shutting down VGUI");
    
    cleanup();
    
    initialized_ = false;
    VGUI_LOG_INFO("Context", "VGUI shutdown complete");
}

bool Context::initializeWindow() {
    auto& window = Window();
    window_ = std::make_shared<Window>();
    
    if (!window_->init(config_.window)) {
        return false;
    }
    
    return true;
}

bool Context::initializeRenderer() {
    // Create device from config
    auto device = Device::create(config_);
    if (!device || !device->isValid()) {
        VGUI_LOG_ERROR("Context", "Failed to create Vulkan device");
        return false;
    }
    
    // Create window surface
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    // Surface creation would go here...
    
    // Create swapchain
    auto swapchain = Swapchain::create(*device, surface, config_);
    if (!swapchain || !swapchain->isValid()) {
        VGUI_LOG_ERROR("Context", "Failed to create swapchain");
        return false;
    }
    
    // Create renderer
    renderer_ = std::make_shared<Renderer>();
    if (!renderer_->initialize(*device, *swapchain, config_)) {
        VGUI_LOG_ERROR("Context", "Failed to initialize renderer");
        return false;
    }
    
    return true;
}

void Context::cleanup() {
    // Cleanup in reverse order
    if (renderer_) {
        renderer_->shutdown();
        renderer_.reset();
    }
    
    if (window_) {
        window_->shutdown();
        window_.reset();
    }
    
    scene_.reset();
    
    VGUI_LOG_INFO("Context", "Context cleanup complete");
}

int Context::run(std::shared_ptr<Application> app) {
    if (!initialized_) {
        VGUI_LOG_ERROR("Context", "Context not initialized");
        return -1;
    }
    
    application_ = app;
    
    // Initialize application
    if (app && !app->onInitialize(*this, *window_, *renderer_)) {
        VGUI_LOG_ERROR("Context", "Application initialization failed");
        return -1;
    }
    
    // Set up frame timing
    DeltaTimer deltaTimer;
    FPSCounter fpsCounter;
    
    VGUI_LOG_INFO("Context", "Entering main loop");
    
    while (!shouldClose_) {
        // Calculate delta time
        float dt = deltaTimer.update();
        
        // Update FPS counter
        fpsCounter.update(dt);
        
        // Process frame
        processFrame();
        
        // Update application
        if (app) {
            app->onUpdate(dt);
        }
        
        // Update scene
        if (scene_) {
            scene_->update(dt);
        }
        
        // Frame timing info
        if (frameCount_ % 60 == 0) {
            VGUI_LOG_DEBUG("Context", 
                "FPS: " << fpsCounter.getFPS() 
                << " Frame time: " << (fpsCounter.getFrameTime() * 1000.0f) << "ms"
                << " Frame: " << frameCount_);
        }
    }
    
    // Cleanup application
    if (app) {
        app->onShutdown();
    }
    
    VGUI_LOG_INFO("Context", "Main loop exited");
    
    return 0;
}

void Context::processFrame() {
    if (!window_ || !renderer_) return;
    
    // Poll events
    window_->pollEvents();
    
    // Check for close
    if (window_->shouldClose()) {
        shouldClose_ = true;
        return;
    }
    
    // Resize check
    uint32_t width, height;
    window_->getFramebufferSize(width, height);
    
    if (width == 0 || height == 0) {
        // Window minimized
        return;
    }
    
    // Update renderer
    renderer_->beginFrame();
    
    // Render scene
    if (scene_) {
        renderer_->render(*scene_, *window_);
    }
    
    // Render UI canvas
    // if (canvas_) {
    //     renderer_->renderUI(*canvas_);
    // }
    
    renderer_->endFrame();
    
    window_->swapBuffers();
    
    frameCount_++;
}

Window& Context::getWindow() {
    if (!window_) {
        throw std::runtime_error("Window not initialized");
    }
    return *window_;
}

Renderer& Context::getRenderer() {
    if (!renderer_) {
        throw std::runtime_error("Renderer not initialized");
    }
    return *renderer_;
}

Scene& Context::getScene() {
    if (!scene_) {
        throw std::runtime_error("Scene not initialized");
    }
    return *scene_;
}

void Context::setScene(std::shared_ptr<Scene> scene) {
    scene_ = scene;
}

bool Context::shouldClose() const {
    return shouldClose_;
}

} // namespace vgui
