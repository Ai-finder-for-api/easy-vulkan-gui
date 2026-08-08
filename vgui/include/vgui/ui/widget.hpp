#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "vgui/math/vec2.hpp"
#include "vgui/math/vec4.hpp"
#include "vgui/math/color.hpp"

namespace vgui {

/**
 * @brief Widget anchor positions
 */
enum class Anchor {
    None = 0,
    TopLeft = 1 << 0,
    TopCenter = 1 << 1,
    TopRight = 1 << 2,
    MiddleLeft = 1 << 3,
    MiddleCenter = 1 << 4,
    MiddleRight = 1 << 5,
    BottomLeft = 1 << 6,
    BottomCenter = 1 << 7,
    BottomRight = 1 << 8,
    Stretch = 1 << 9  // Stretch to fill parent
};

/**
 * * @brief Widget pivot point
 */
enum class Pivot {
    TopLeft = 0,
    TopCenter = 1,
    TopRight = 2,
    MiddleLeft = 3,
    MiddleCenter = 4,
    MiddleRight = 5,
    BottomLeft = 6,
    BottomCenter = 7,
    BottomRight = 8
};

/**
 * @brief Widget visibility
 */
enum class Visibility {
    Visible = 0,
    Hidden = 1,
    Collapsed = 2  // Hidden and doesn't take layout space
};

/**
 * @brief Widget state
 */
enum class WidgetState {
    Normal = 0,
    Hover = 1,
    Active = 2,
    Focused = 4,
    Disabled = 8,
    Checked = 16,
    Selected = 32
};

/**
 * @brief Widget flags
 */
enum class WidgetFlag {
    None = 0,
    AutoSize = 1 << 0,      // Widget sizes itself to content
    StretchWidth = 1 << 1,  // Widget stretches to fill width
    StretchHeight = 1 << 2, // Widget stretches to fill height
    ContainsFocus = 1 << 3, // Widget currently has input focus
    TabStop = 1 << 4,       // Widget can receive tab focus
    Visible = 1 << 5,       // Widget is visible
    Enabled = 1 << 6,       // Widget is enabled
    LayoutDirty = 1 << 7,   // Layout needs updating
};

/**
 * @brief Click mode
 */
enum class ClickMode {
    Press,
    Release,
    Toggle,
    Switch
};

/**
 * @brief Widget event types
 */
enum class WidgetEventType {
    Click,
    DoubleClick,
    RightClick,
    HoverEnter,
    HoverLeave,
    FocusEnter,
    FocusLeave,
    TextChanged,
    ValueChanged,
    SelectionChanged,
    Scroll,
    Custom
};

/**
 * @brief Widget event
 */
struct WidgetEvent {
    WidgetEventType type = WidgetEventType::Custom;
    std::shared_ptr<class Widget> widget;
    float x = 0.0f, y = 0.0f;
    float deltaX = 0.0f, deltaY = 0.0f;
    float scrollDelta = 0.0f;
    std::string text;
    bool value = false;
    int32_t intValue = 0;
    float floatValue = 0.0f;
    void* customData = nullptr;
};

/**
 * @brief Widget event callback
 */
using WidgetEventCallback = std::function<void(const WidgetEvent&)>;

/**
 * @brief Widget class
 * 
 * Base class for all UI widgets. Provides position, size, styling,
 * and event handling.
 */
class Widget {
public:
    /**
     * @brief Create a widget
     */
    Widget();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Widget();
    
    /**
     * @brief Check if widget is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get widget name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set widget name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Get widget ID
     */
    uint64_t getId() const { return id_; }
    
    /**
     * @brief Get canvas
     */
    Canvas* getCanvas() const { return canvas_; }
    
    /**
     * @brief Set canvas
     */
    virtual void setCanvas(Canvas* canvas);
    
    /**
     * @brief Get position (relative to parent)
     */
    Vec2f getPosition() const { return position_; }
    
    /**
     * @brief Set position
     */
    virtual void setPosition(float x, float y);
    
    /**
     * @brief Get size
     */
    Vec2f getSize() const { return size_; }
    
    /**
     * @brief Set size
     */
    virtual void setSize(float width, float height);
    
    /**
     * @brief Get desired size (for layout)
     */
    virtual Vec2f getDesiredSize() const;
    
    /**
     * @brief Get actual size (after layout)
     */
    Vec2f getActualSize() const { return actualSize_; }
    
    /**
     * @brief Get minimum size
     */
    Vec2f getMinSize() const { return minSize_; }
    
    /**
     * @brief Set minimum size
     */
    void setMinSize(float width, float height);
    
    /**
     * @brief Get maximum size
     */
    Vec2f getMaxSize() const { return maxSize_; }
    
    /**
     * @brief Set maximum size
     */
    void setMaxSize(float width, float height);
    
    /**
     * @brief Get margin
     */
    Vec4f getMargin() const { return margin_; }
    
    /**
     * @brief Set margin
     */
    void setMargin(float left, float top, float right, float bottom);
    
    /**
     * @brief Get padding
     */
    Vec4f getPadding() const { return padding_; }
    
    /**
     * @brief Set padding
     */
    void setPadding(float left, float top, float right, float bottom);
    
    /**
     * @brief Get anchor
     */
    Anchor getAnchor() const { return anchor_; }
    
    /**
     * @brief Set anchor
     */
    void setAnchor(Anchor anchor);
    
    /**
     * @brief Get pivot
     */
    Pivot getPivot() const { return pivot_; }
    
    /**
     * @brief Set pivot
     */
    void setPivot(Pivot pivot);
    
    /**
     * @brief Get visibility
     */
    Visibility getVisibility() const { return visibility_; }
    
    /**
     * @brief Set visibility
     */
    void setVisibility(Visibility visibility);
    
    /**
     * @brief Check if visible
     */
    bool isVisible() const { return visibility_ == Visibility::Visible; }
    
    /**
     * @brief Check if enabled
     */
    bool isEnabled() const { return (flags_ & static_cast<uint32_t>(WidgetFlag::Enabled)) != 0; }
    
    /**
     * @brief Set enabled state
     */
    virtual void setEnabled(bool enabled);
    
    /**
     * @brief Check if widget has focus
     */
    bool hasFocus() const { return (flags_ & static_cast<uint32_t>(WidgetFlag::ContainsFocus)) != 0; }
    
    /**
     * @brief Get widget state
     */
    uint32_t getState() const { return state_; }
    
    /**
     * @brief Check if widget is in a specific state
     */
    bool isState(uint32_t state) const { return (state_ & state) != 0; }
    
    /**
     * @brief Check if hovered
     */
    bool isHovered() const { return isState(static_cast<uint32_t>(WidgetState::Hover)); }
    
    /**
     * @brief Check if active
     */
    bool isActive() const { return isState(static_cast<uint32_t>(WidgetState::Active)); }
    
    /**
     * @brief Check if focused
     */
    bool isFocused() const { return isState(static_cast<uint32_t>(WidgetState::Focused)); }
    
    /**
     * @brief Check if disabled
     */
    bool isDisabled() const { return isState(static_cast<uint32_t>(WidgetState::Disabled)); }
    
    /**
     * @brief Get parent widget
     */
    std::shared_ptr<Widget> getParent() const { return parent_; }
    
    /**
     * @brief Get children
     */
    const std::vector<std::shared_ptr<Widget>>& getChildren() const { return children_; }
    
    /**
     * @brief Get child count
     */
    size_t getChildCount() const { return children_.size(); }
    
    /**
     * @brief Get world position
     */
    Vec2f getWorldPosition() const;
    
    /**
     * @brief Get world rect
     */
    Rectf getWorldRect() const;
    
    /**
     * @brief Check if point is inside widget
     */
    virtual bool containsPoint(float x, float y) const;
    
    /**
     * @brief Hit test (check if point hits this widget)
     */
    virtual bool hitTest(float x, float y);
    
    /**
     * @brief Set focus to this widget
     */
    virtual void setFocus(bool focus);
    
    /**
     * @brief Request focus
     */
    virtual void requestFocus();
    
    /**
     * @brief Set opacity
     */
    void setOpacity(float opacity);
    
    /**
     * @brief Get opacity
     */
    float getOpacity() const { return opacity_; }
    
    /**
     * @brief Set color tint
     */
    void setTint(const Colorf& tint);
    
    /**
     * @brief Get tint
     */
    Colorf getTint() const { return tint_; }
    
    /**
     * @brief Get normal color
     */
    Colorf getNormalColor() const { return normalColor_; }
    
    /**
     * @brief Set normal color
     */
    void setNormalColor(const Colorf& color);
    
    /**
     * @brief Get hover color
     */
    Colorf getHoverColor() const { return hoverColor_; }
    
    /**
     * @brief Set hover color
     */
    void setHoverColor(const Colorf& color);
    
    /**
     * @brief Get active color
     */
    Colorf getActiveColor() const { return activeColor_; }
    
    /**
     * @brief Set active color
     */
    void setActiveColor(const Colorf& color);
    
    /**
     * @brief Get disabled color
     */
    Colorf getDisabledColor() const { return disabledColor_; }
    
    /**
     * @brief Set disabled color
     */
    void setDisabledColor(const Colorf& color);
    
    /**
     * @brief Get border color
     */
    Colorf getBorderColor() const { return borderColor_; }
    
    /**
     * @brief Set border color
     */
    void setBorderColor(const Colorf& color);
    
    /**
     * @brief Get border width
     */
    float getBorderWidth() const { return borderWidth_; }
    
    /**
     * @brief Set border width
     */
    void setBorderWidth(float width);
    
    /**
     * @brief Get corner rounding
     */
    float getCornerRounding() const { return cornerRounding_; }
    
    /**
     * @brief Set corner rounding
     */
    void setCornerRounding(float radius);
    
    /**
     * @brief Get background image
     */
    std::shared_ptr<class Texture> getBackgroundImage() const { return backgroundImage_; }
    
    /**
     * @brief Set background image
     */
    void setBackgroundImage(std::shared_ptr<class Texture> texture);
    
    /**
     * @brief Get background image color
     */
    Colorf getBackgroundImageColor() const { return backgroundImageColor_; }
    
    /**
     * @brief Set background image color
     */
    void setBackgroundImageColor(const Colorf& color);
    
    /**
     * @brief Set layout
     */
    void setLayout(std::shared_ptr<class Layout> layout);
    
    /**
     * @brief Get layout
     */
    std::shared_ptr<class Layout> getLayout() const { return layout_; }
    
    /**
     * @brief Add child widget
     */
    virtual void addChild(std::shared_ptr<Widget> child);
    
    /**
     * @brief Insert child at position
     */
    virtual void insertChild(size_t index, std::shared_ptr<Widget> child);
    
    /**
     * @brief Remove child
     */
    virtual void removeChild(std::shared_ptr<Widget> child);
    
    /**
     * @brief Remove all children
     */
    virtual void removeAllChildren();
    
    /**
     * @brief Get child at index
     */
    std::shared_ptr<Widget> getChild(size_t index) const;
    
    /**
     * @brief Find child by name
     */
    std::shared_ptr<Widget> findChild(const std::string& name) const;
    
    /**
     * @brief Find child by ID
     */
    std::shared_ptr<Widget> findChildById(uint64_t id) const;
    
    /**
     * @brief Get parent panel
     */
    std::shared_ptr<class Panel> getParentPanel() const;
    
    /**
     * @brief Get root canvas
     */
    Canvas* getRootCanvas() const;
    
    /**
     * @brief Set click mode
     */
    void setClickMode(ClickMode mode);
    
    /**
     * @brief Get click mode
     */
    ClickMode getClickMode() const { return clickMode_; }
    
    /**
     * @brief Check if clicked
     */
    bool isClicked() const { return clicked_; }
    
    /**
     * @brief Set clicked state
     */
    void setClicked(bool clicked);
    
    /**
     * @brief Bind click event
     */
    void onClick(WidgetEventCallback callback);
    
    /**
     * @brief Bind double click event
     */
    void onDoubleClick(WidgetEventCallback callback);
    
    /**
     * @brief Bind hover enter event
     */
    void onHoverEnter(WidgetEventCallback callback);
    
    /**
     * @brief Bind hover leave event
     */
    void onHoverLeave(WidgetEventCallback callback);
    
    /**
     * @brief Bind focus enter event
     */
    void onFocusEnter(WidgetEventCallback callback);
    
    /**
     * @brief Bind focus leave event
     */
    void onFocusLeave(WidgetEventCallback callback);
    
    /**
     * @brief Bind scroll event
     */
    void onScroll(WidgetEventCallback callback);
    
    /**
     * @brief Bind custom event
     */
    void onCustom(WidgetEventCallback callback);
    
    /**
     * @brief Fire event
     */
    void fireEvent(const WidgetEvent& event);
    
    /**
     * @brief Send event to this widget
     */
    void sendEvent(const WidgetEvent& event);
    
    /**
     * @brief Handle mouse down
     */
    virtual bool handleMouseDown(float x, float y, uint32_t button);
    
    /**
     * @brief Handle mouse up
     */
    virtual bool handleMouseUp(float x, float y, uint32_t button);
    
    /**
     * @brief Handle mouse move
     */
    virtual bool handleMouseMove(float x, float y);
    
    /**
     * @brief Handle mouse wheel
     */
    virtual bool handleMouseWheel(float delta);
    
    /**
     * @brief Handle key down
     */
    virtual bool handleKeyDown(uint32_t key, uint32_t mods);
    
    /**
     * @brief Handle key up
     */
    virtual bool handleKeyUp(uint32_t key, uint32_t mods);
    
    /**
     * @brief Handle character input
     */
    virtual bool handleCharInput(uint32_t character);
    
    /**
     * @brief Render widget
     */
    virtual void render(class CommandBuffer& cmdBuffer,
                       const Mat4f& transform,
                       const Colorf& colorOverride = Colorf::WHITE);
    
    /**
     * @brief Layout widget
     */
    virtual void layout(float x, float y, float width, float height);
    
    /**
     * @brief Layout children
     */
    virtual void layoutChildren();
    
    /**
     * @brief Get desired width for autofit content
     */
    virtual float getContentWidth() const { return 0.0f; }
    
    /**
     * @brief Get desired height for autofit content
     */
    virtual float getContentHeight() const { return 0.0f; }
    
    /**
     * @brief Get text content (for text widgets)
     */
    virtual std::string getText() const { return text_; }
    
    /**
     * @brief Set text content
     */
    virtual void setText(const std::string& text);
    
    /**
     * @brief Get tooltip text
     */
    const std::string& getTooltip() const { return tooltip_; }
    
    /**
     * @brief Set tooltip text
     */
    void setTooltip(const std::string& tooltip);
    
    /**
     * @brief Update widget (per-frame)
     */
    virtual void update(float deltaTime);
    
    /**
     * @brief Clear callback bindings
     */
    void clearBindings();
    
    /**
     * @brief Get widget flags
     */
    uint32_t getFlags() const { return flags_; }
    
    /**
     * @brief Set widget flags
     */
    void setFlags(uint32_t flags);
    
    /**
     * @brief Check if flag is set
     */
    bool hasFlag(WidgetFlag flag) const { 
        return (flags_ & static_cast<uint32_t>(flag)) != 0; 
    }
    
    /**
     * @brief Set flag
     */
    void setFlag(WidgetFlag flag, bool value);
    
    /**
     * @brief Get z-order
     */
    int32_t getZOrder() const { return zOrder_; }
    
    /**
     * @brief Set z-order
     */
    void setZOrder(int32_t z);
    
    /**
     * @brief Check if widget is dirty
     */
    bool isDirty() const { return dirty_; }
    
    /**
     * @brief Mark widget as dirty
     */
    void setDirty(bool dirty);
    
    /**
     * @brief Invalidate layout
     */
    void invalidateLayout();
    
    /**
     * @brief Invalidate render
     */
    void invalidateRender();
    
protected:
    virtual void onMouseDown(float x, float y, uint32_t button);
    virtual void onMouseUp(float x, float y, uint32_t button);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseWheel(float delta);
    virtual void onKeyDown(uint32_t key, uint32_t mods);
    virtual void onKeyUp(uint32_t key, uint32_t mods);
    virtual void onCharInput(uint32_t character);
    virtual void onFocus(bool focused);
    virtual void onHover(bool hovered);
    virtual void onActivate(bool activated);
    virtual void onRender(class CommandBuffer& cmdBuffer,
                         const Mat4f& transform,
                         const Colorf& colorOverride);
    
    virtual void onLayoutChange();
    virtual void onSizeChange();
    virtual void onVisibilityChange(Visibility visibility);
    
    void propagateToChildren(const std::function<void(std::shared_ptr<Widget>)>& fn);
    
    // Generate unique ID
    static uint64_t generateId();
    
    uint64_t id_;
    bool valid_ = false;
    
    std::string name_;
    std::string text_;
    std::string tooltip_;
    
    Canvas* canvas_ = nullptr;
    std::shared_ptr<Widget> parent_;
    std::vector<std::shared_ptr<Widget>> children_;
    
    Vec2f position_ = Vec2f::ZERO;
    Vec2f size_ = Vec2f(100.0f, 30.0f);
    Vec2f actualSize_ = Vec2f::ZERO;
    Vec2f minSize_ = Vec2f::ZERO;
    Vec2f maxSize_ = Vec2f(FLT_MAX, FLT_MAX);
    Vec4f margin_ = Vec4f::ZERO;
    Vec4f padding_ = Vec4f::ZERO;
    
    Anchor anchor_ = Anchor::TopLeft;
    Pivot pivot_ = Pivot::TopLeft;
    Visibility visibility_ = Visibility::Visible;
    uint32_t flags_ = static_cast<uint32_t>(WidgetFlag::Enabled) |
                       static_cast<uint32_t>(WidgetFlag::Visible);
    uint32_t state_ = 0;
    
    float opacity_ = 1.0f;
    Colorf tint_ = Colorf::WHITE;
    
    Colorf normalColor_ = Colorf(0.25f, 0.25f, 0.3f);
    Colorf hoverColor_ = Colorf(0.35f, 0.35f, 0.45f);
    Colorf activeColor_ = Colorf(0.45f, 0.45f, 0.55f);
    Colorf disabledColor_ = Colorf(0.15f, 0.15f, 0.2f);
    Colorf borderColor_ = Colorf(0.3f, 0.3f, 0.35f);
    float borderWidth_ = 1.0f;
    float cornerRounding_ = 4.0f;
    
    std::shared_ptr<class Texture> backgroundImage_;
    Colorf backgroundImageColor_ = Colorf::WHITE;
    
    std::shared_ptr<class Layout> layout_;
    
    ClickMode clickMode_ = ClickMode::Press;
    bool clicked_ = false;
    
    int32_t zOrder_ = 0;
    bool dirty_ = true;
    
    // Events
    WidgetEventCallback clickCallback_;
    WidgetEventCallback doubleClickCallback_;
    WidgetEventCallback hoverEnterCallback_;
    WidgetEventCallback hoverLeaveCallback_;
    WidgetEventCallback focusEnterCallback_;
    WidgetEventCallback focusLeaveCallback_;
    WidgetEventCallback scrollCallback_;
    WidgetEventCallback customCallback_;
    
    // Internal cache
    mutable Vec2f cachedDesiredSize_ = Vec2f::ZERO;
    bool sizeCacheValid_ = false;
};

} // namespace vgui
