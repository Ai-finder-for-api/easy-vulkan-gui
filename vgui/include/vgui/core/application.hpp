#pragma once

#include <memory>
#include <string>
#include <functional>
#include "vgui/core/config.hpp"

namespace vgui {

// Forward declarations
class Context;
class Window;
class Renderer;
class Scene;

/**
 * @brief Application base class
 * 
 * Derive from this class to create a VGUI application.
 * Override the lifecycle methods to implement your application logic.
 */
class Application {
public:
    /**
     * @brief Constructor
     */
    Application() = default;
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Application() = default;
    
    /**
     * @brief Get the application name
     */
    virtual std::string getName() const { return name_; }
    
    /**
     * @brief Set the application name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Get the application version
     */
    virtual uint32_t getMajorVersion() const { return 1; }
    virtual uint32_t getMinorVersion() const { return 0; }
    virtual uint32_t getPatchVersion() const { return 0; }
    
    /**
     * @brief Called when the application is initialized
     * @param context The VGUI context
     * @param window The main window
     * @param renderer The renderer
     * @return true on success, false to abort initialization
     */
    virtual bool onInitialize(Context& context, Window& window, Renderer& renderer) {
        VGUI_LOG_INFO("Application", "Initialized: " + getName());
        return true;
    }
    
    /**
     * @brief Called when the application is about to shutdown
     */
    virtual void onShutdown() {
        VGUI_LOG_INFO("Application", "Shutting down: " + getName());
    }
    
    /**
     * @brief Called once per frame before rendering
     * @param deltaTime Time since last frame in seconds
     */
    virtual void onUpdate(double deltaTime) {
        // Override to implement update logic
    }
    
    /**
     * @brief Called once per frame after update, before rendering
     */
    virtual void onPreRender() {
        // Override for pre-rendering setup
    }
    
    /**
     * @brief Called once per frame after rendering
     */
    virtual void onPostRender() {
        // Override for post-rendering cleanup
    }
    
    /**
     * @brief Called when the window is resized
     * @param width New width
     * @param height New height
     */
    virtual void onResize(uint32_t width, uint32_t height) {
        // Override to handle resize
    }
    
    /**
     * @brief Called when a key is pressed
     * @param key Key code
     * @param scancode Platform-specific scancode
     * @param action GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT
     * @param mods Modifier keys
     */
    virtual void onKey(uint32_t key, uint32_t scancode, uint32_t action, uint32_t mods) {
        // Override to handle keyboard input
        if (key == 256 /*ESC*/ && action == 1 /*PRESS*/) {
            Context::getInstance().requestClose();
        }
    }
    
    /**
     * @brief Called when the mouse is moved
     * @param x Pointer x position
     * @param y Pointer y position
     */
    virtual void onMouseMove(double x, double y) {
        // Override to handle mouse movement
    }
    
    /**
     * @brief Called when a mouse button is pressed/released
     * @param button Mouse button
     * @param action GLFW_PRESS or GLFW_RELEASE
     * @param mods Modifier keys
     */
    virtual void onMouseButton(uint32_t button, uint32_t action, uint32_t mods) {
        // Override to handle mouse buttons
    }
    
    /**
     * @brief Called when mouse scroll occurs
     * @param xoffset X scroll offset
     * @param yoffset Y scroll offset
     */
    virtual void onScroll(double xoffset, double yoffset) {
        // Override to handle scroll
    }
    
    /**
     * @brief Called when a character is typed
     * @param character Unicode character
     */
    virtual void onChar(uint32_t character) {
        // Override to handle text input
    }
    
    /**
     * @brief Get the main scene
     * @return Shared pointer to the scene, or nullptr if not set
     */
    virtual std::shared_ptr<Scene> getScene() {
        return nullptr;
    }
    
    /**
     * @brief Set the main scene
     */
    virtual void setScene(std::shared_ptr<Scene> scene) {
        // Override to store scene reference
    }
    
protected:
    std::string name_ = "Unnamed Application";
};

/**
 * @brief Simple application template for quick prototyping
 * 
 * Usage:
 * @code
 * struct MyApp : SimpleApplication {
 *     void onUpdate(double dt) override {
 *         // Your update logic
 *     }
 * };
 * 
 * int main() {
 *     MyApp app;
 *     Context::getInstance().run(app);
 *     return 0;
 * }
 * @endcode
 */
template<typename Derived>
class SimpleApplication : public Application {
public:
    Derived& self() { return static_cast<Derived&>(*this); }
    
    bool onInitialize(Context& context, Window& window, Renderer& renderer) override {
        if constexpr (requires { self().onInitialize(context, window, renderer); }) {
            return self().onInitialize(context, window, renderer);
        }
        return true;
    }
    
    void onUpdate(double deltaTime) override {
        if constexpr (requires { self().onUpdate(deltaTime); }) {
            self().onUpdate(deltaTime);
        }
    }
    
    void onKey(uint32_t key, uint32_t scancode, uint32_t action, uint32_t mods) override {
        if constexpr (requires { self().onKey(key, scancode, action, mods); }) {
            self().onKey(key, scancode, action, mods);
        }
        Application::onKey(key, scancode, action, mods);
    }
};

} // namespace vgui
