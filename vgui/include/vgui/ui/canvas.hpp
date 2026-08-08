#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "vgui/ui/widget.hpp"
#include "vgui/ui/layout.hpp"
#include "vgui/ui/font.hpp"
#include "vgui/math/vec2.hpp"
#include "vgui/math/color.hpp"

namespace vgui {

/**
 * @brief Canvas event types
 */
enum class CanvasEventType {
    MouseDown,
    MouseUp,
    MouseMove,
    MouseWheel,
    KeyDown,
    KeyUp,
    CharInput,
    FocusChange,
    Resize,
    Render,
    Custom
};

/**
 * @brief Canvas event
 */
struct CanvasEvent {
    CanvasEventType type = CanvasEventType::Custom;
    Vec2f mousePosition = Vec2f::ZERO;
    Vec2f mouseDelta = Vec2f::ZERO;
    float scrollDelta = 0.0f;
    uint32_t key = 0;
    uint32_t action = 0;
    uint32_t mods = 0;
    uint32_t character = 0;
    void* customData = nullptr;
    
    template<typename T>
    T* getCustomData() { return static_cast<T*>(customData); }
};

/**
 * @brief Canvas event callback
 */
using CanvasEventCallback = std::function<void(const CanvasEvent&)>;

/**
 * @brief Canvas class
 * 
 * Root UI container that manages widgets and handles input.
 * Acts as the main entry point for UI rendering.
 */
class Canvas {
public:
    /**
     * @brief Create an empty canvas
     */
    Canvas();
    
    /**
     * @brief Destroy the canvas
     */
    ~Canvas();
    
    /**
     * @brief Check if canvas is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Initialize canvas with viewport size
     */
    bool initialize(uint32_t width, uint32_t height);
    
    /**
     * @brief Shutdown canvas
     */
    void shutdown();
    
    /**
     * @brief Set viewport size
     */
    void setViewportSize(uint32_t width, uint32_t height);
    
    /**
     * @brief Get viewport size
     */
    Vec2u getViewportSize() const { return Vec2u(viewportWidth_, viewportHeight_); }
    
    /**
     * @brief Set canvas size (for scaling)
     */
    void setCanvasSize(float width, float height);
    
    /**
     * @brief Get canvas size
     */
    Vec2f getCanvasSize() const { return canvasSize_; }
    
    /**
     * @brief Set scaling mode
     */
    void setScalingMode(ScalingMode mode);
    
    /**
     * @brief Get scaling mode
     */
    ScalingMode getScalingMode() const { return scalingMode_; }
    
    /**
     * @brief Get canvas pixel ratio
     */
    float getPixelRatio() const { return pixelRatio_; }
    
    /**
     * @brief Get root panel
     */
    Panel& getRootPanel() { return rootPanel_; }
    const Panel& getRootPanel() const { return rootPanel_; }
    
    /**
     * @brief Add widget to canvas
     */
    template<typename T, typename... Args>
    std::shared_ptr<T> addWidget(Args&&... args) {
        static_assert(std::is_base_of<Widget, T>::value, 
                     "T must derive from Widget");
        auto widget = std::make_shared<T>(std::forward<Args>(args)...);
        widget->setCanvas(this);
        rootPanel_.addChild(widget);
        return widget;
    }
    
    /**
     * @brief Remove widget
     */
    void removeWidget(std::shared_ptr<Widget> widget);
    
    /**
     * @brief Find widget by name
     */
    std::shared_ptr<Widget> findWidget(const std::string& name) const;
    
    /**
     * @brief Find widget by ID
     */
    std::shared_ptr<Widget> findWidgetById(uint64_t id) const;
    
    /**
     * @brief Get all widgets
     */
    const std::vector<std::shared_ptr<Widget>>& getWidgets() const { return widgets_; }
    
    /**
     * @brief Get widget count
     */
    size_t getWidgetCount() const { return widgets_.size(); }
    
    /**
     * @brief Set font
     */
    void setFont(std::shared_ptr<Font> font);
    
    /**
     * @brief Get font
     */
    std::shared_ptr<Font> getFont() const { return font_; }
    
    /**
     * @brief Set default font size
     */
    void setDefaultFontSize(float size);
    
    /**
     * @brief Get default font size
     */
    float getDefaultFontSize() const { return defaultFontSize_; }
    
    /**
     * @brief Set background color
     */
    void setBackgroundColor(const Colorf& color);
    
    /**
     * @brief Get background color
     */
    Colorf getBackgroundColor() const { return backgroundColor_; }
    
    /**
     * @brief Check if canvas is rendering
     */
    bool isRendering() const { return rendering_; }
    
    /**
     * @brief Render canvas
     */
    void render(class CommandBuffer& cmdBuffer,
               const Mat4f& transform,
               const Colorf& colorOverride = Colorf::WHITE);
    
    /**
     * @brief Handle mouse event
     */
    bool handleMouseDown(uint32_t button, float x, float y);
    bool handleMouseUp(uint32_t button, float x, float y);
    bool handleMouseMove(float x, float y);
    bool handleMouseWheel(float delta);
    
    /**
     * @brief Handle keyboard event
     */
    bool handleKeyDown(uint32_t key, uint32_t mods);
    bool handleKeyUp(uint32_t key, uint32_t mods);
    bool handleCharInput(uint32_t character);
    
    /**
     * @brief Set event callback
     */
    void setEventCallback(CanvasEventCallback callback);
    
    /**
     * @brief Capture mouse to specific widget
     */
    void captureMouse(std::shared_ptr<Widget> widget);
    void releaseMouse();
    
    /**
     * @brief Capture keyboard to specific widget
     */
    void captureKeyboard(std::shared_ptr<Widget> widget);
    void releaseKeyboard();
    
    /**
     * @brief Get captured mouse widget
     */
    std::shared_ptr<Widget> getCapturedMouseWidget() const { return capturedMouseWidget_; }
    
    /**
     * @brief Get captured keyboard widget
     */
    std::shared_ptr<Widget> getCapturedKeyboardWidget() const { return capturedKeyboardWidget_; }
    
    /**
     * @brief Get canvas name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set canvas name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Clear canvas
     */
    void clear();
    
    /**
     * @brief Create default canvas
     */
    static std::shared_ptr<Canvas> create(uint32_t width = 1280, 
                                          uint32_t height = 720);
    
private:
    bool handleEvent(const CanvasEvent& event);
    void propagateEvent(const CanvasEvent& event, 
                       std::shared_ptr<Widget> widget);
    bool hitTest(float x, float y, std::shared_ptr<Widget> widget);
    
    void updateLayout();
    
    bool valid_ = false;
    std::string name_ = "Canvas";
    
    uint32_t viewportWidth_ = 1280;
    uint32_t viewportHeight_ = 720;
    float canvasWidth_ = 1280.0f;
    float canvasHeight_ = 720.0f;
    float pixelRatio_ = 1.0f;
    
    ScalingMode scalingMode_ = ScalingMode::ScaleAndKeepAspect;
    
    Vec2f canvasSize_;
    Colorf backgroundColor_ = Colorf(0.08f, 0.08f, 0.1f);
    
    Panel rootPanel_;
    std::shared_ptr<Font> font_;
    float defaultFontSize_ = 14.0f;
    
    std::vector<std::shared_ptr<Widget>> widgets_;
    std::unordered_map<std::string, std::shared_ptr<Widget>> widgetMap_;
    std::unordered_map<uint64_t, std::shared_ptr<Widget>> widgetIdMap_;
    
    std::shared_ptr<Widget> capturedMouseWidget_;
    std::shared_ptr<Widget> capturedKeyboardWidget_;
    uint32_t capturedMouseButton_ = 0;
    
    CanvasEventCallback eventCallback_;
    
    bool rendering_ = false;
    
    // Dirty flags
    bool needsLayout_ = true;
    bool needsRender_ = true;
};

} // namespace vgui
