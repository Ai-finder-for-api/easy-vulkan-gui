#pragma once

#include <string>
#include <memory>
#include <vector>

#include "vgui/ui/widget.hpp"

namespace vgui {

/**
 * @brief Panel widget
 * 
 * A container widget that can hold other widgets.
 * Provides layout management and visual styling.
 */
class Panel : public Widget {
public:
    /**
     * @brief Create a panel
     */
    Panel();
    
    /**
     * @brief Create panel with size
     */
    Panel(float width, float height);
    
    /**
     * @brief Add child widget
     */
    void addChild(std::shared_ptr<Widget> child) override;
    
    /**
     * @brief Insert child at position
     */
    void insertChild(size_t index, std::shared_ptr<Widget> child) override;
    
    /**
     * @brief Remove child
     */
    void removeChild(std::shared_ptr<Widget> child) override;
    
    /**
     * @brief Remove all children
     */
    void removeAllChildren() override;
    
    /**
     * @brief Get child at index
     */
    std::shared_ptr<Widget> getChild(size_t index) const override;
    
    /**
     * @brief Find child by name
     */
    std::shared_ptr<Widget> findChild(const std::string& name) const override;
    
    /**
     * @brief Get child count
     */
    size_t getChildCount() const override { return children_.size(); }
    
    /**
     * @brief Get desired size
     */
    Vec2f getDesiredSize() const override;
    
    /**
     * @brief Get content size
     */
    Vec2f getContentSize() const override;
    
    /**
     * @brief Set layout
     */
    void setLayout(std::shared_ptr<class Layout> layout) override;
    
    /**
     * @brief Get layout
     */
    std::shared_ptr<class Layout> getLayout() const override { return layout_; }
    
    /**
     * @brief Clear layout
     */
    void clearLayout();
    
    /**
     * @brief Get child padding
     */
    Vec4f getChildPadding() const { return childPadding_; }
    
    /**
     * @brief Set child padding
     */
    void setChildPadding(float left, float top, float right, float bottom);
    
    /**
     * @brief Set spacing between children
     */
    void setSpacing(float spacing);
    
    /**
     * @brief Get spacing
     */
    float getSpacing() const { return spacing_; }
    
    /**
     * @brief Set direction lock
     */
    void setDirectionLock(bool locked);
    
    /**
     * @brief Check if direction is locked
     */
    bool isDirectionLocked() const { return directionLocked_; }
    
    /**
     * @brief Layout children
     */
    void layoutChildren() override;
    
    /**
     * @brief Render panel
     */
    void render(class CommandBuffer& cmdBuffer,
               const Mat4f& transform,
               const Colorf& colorOverride = Colorf::WHITE) override;
    
    /**
     * @brief Hit test
     */
    bool hitTest(float x, float y) override;
    
    /**
     * @brief Get panel background
     */
    std::shared_ptr<class Texture> getBackground() const { return background_; }
    
    /**
     * @brief Set panel background
     */
    void setBackground(std::shared_ptr<class Texture> texture);
    
    /**
     * @brief Set background color
     */
    void setBackgroundColor(const Colorf& color);
    
    /**
     * @brief Get background color
     */
    Colorf getBackgroundColor() const { return backgroundColor_; }
    
    /**
     * @brief Set border color
     */
    void setBorderColor(const Colorf& color);
    
    /**
     * @brief Get border color
     */
    Colorf getBorderColor() const { return borderColor_; }
    
    /**
     * @brief Set border width
     */
    void setBorderWidth(float width);
    
    /**
     * @brief Get border width
     */
    float getBorderWidth() const { return borderWidth_; }
    
    /**
     * @brief Set corner rounding
     */
    void setCornerRounding(float radius);
    
    /**
     * @brief Get corner rounding
     */
    float getCornerRounding() const { return cornerRounding_; }
    
    /**
     * @brief Set shadow
     */
    void setShadow(bool enabled, const Vec2f& offset = Vec2f(2.0f, 2.0f),
                   const Colorf& color = Colorf(0.0f, 0.0f, 0.0f, 0.3f),
                   float blur = 4.0f);
    
    /**
     * @brief Check if shadow is enabled
     */
    bool hasShadow() const { return shadowEnabled_; }
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
    void onLayoutChange() override;
    
    void onChildAdd(std::shared_ptr<Widget> child);
    void onChildRemove(std::shared_ptr<Widget> child);
    
    std::shared_ptr<class Layout> layout_;
    
    // Visual styling
    std::shared_ptr<class Texture> background_;
    Colorf backgroundColor_ = Colorf(0.0f, 0.0f, 0.0f, 0.0f);
    Colorf borderColor_ = Colorf(0.2f, 0.2f, 0.2f);
    float borderWidth_ = 1.0f;
    float cornerRounding_ = 4.0f;
    
    // Child layout
    Vec4f childPadding_ = Vec4f(4.0f);
    float spacing_ = 4.0f;
    bool directionLocked_ = false;
    bool autoLayout_ = true;
    
    // Shadow
    bool shadowEnabled_ = false;
    Vec2f shadowOffset_ = Vec2f(2.0f, 2.0f);
    Colorf shadowColor_ = Colorf(0.0f, 0.0f, 0.0f, 0.3f);
    float shadowBlur_ = 4.0f;
    
    // Cached layout info
    Vec2f contentSize_ = Vec2f::ZERO;
    bool contentSizeValid_ = false;
    
    friend class Canvas;
    friend class Layout;
};

/**
 * @brief Collapsible panel
 */
class CollapsiblePanel : public Panel {
public:
    /**
     * @brief Create collapsible panel
     */
    CollapsiblePanel();
    
    /**
     * @brief Create with title
     */
    CollapsiblePanel(const std::string& title);
    
    /**
     * @brief Set title
     */
    void setTitle(const std::string& title);
    
    /**
     * @brief Get title
     */
    std::string getTitle() const { return title_; }
    
    /**
     * @brief Check if expanded
     */
    bool isExpanded() const { return expanded_; }
    
    /**
     * @brief Set expanded
     */
    void setExpanded(bool expanded);
    
    /**
     * @brief Toggle expanded
     */
    void toggle();
    
    /**
     * @brief Set title bar height
     */
    void setTitleBarHeight(float height);
    
    /**
     * @brief Get title bar height
     */
    float getTitleBarHeight() const { return titleBarHeight_; }
    
    /**
     * @brief Set collapsed height
     */
    void setCollapsedHeight(float height);
    
    /**
     * @brief Get collapsed height
     */
    float getCollapsedHeight() const { return collapsedHeight_; }
    
    /**
     * @brief Set title text color
     */
    void setTitleColor(const Colorf& color);
    
    /**
     * @brief Get title color
     */
    Colorf getTitleColor() const { return titleColor_; }
    
    /**
     * @brief Set title background color
     */
    void setTitleBackgroundColor(const Colorf& color);
    
    /**
     * @brief Get title background color
     */
    Colorf getTitleBackgroundColor() const { return titleBackgroundColor_; }
    
    /**
     * @brief Get desired size
     */
    Vec2f getDesiredSize() const override;
    
    /**
     * @brief Get content size
     */
    Vec2f getContentSize() const override;
    
    /**
     * @brief Handle mouse down
     */
    bool handleMouseDown(float x, float y, uint32_t button) override;
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
    void onLayoutChange() override;
    
private:
    std::string title_;
    bool expanded_ = true;
    float titleBarHeight_ = 28.0f;
    float collapsedHeight_ = 28.0f;
    Colorf titleColor_ = Colorf(1.0f, 1.0f, 1.0f);
    Colorf titleBackgroundColor_ = Colorf(0.15f, 0.15f, 0.18f);
    
    Vec2f desiredSizeCache_ = Vec2f::ZERO;
    bool desiredSizeCacheValid_ = false;
    
    std::shared_ptr<Button> titleButton_;
    
    friend class Canvas;
};

/**
 * @brief Scrollable panel
 */
class ScrollablePanel : public Panel {
public:
    /**
     * @brief Create scrollable panel
     */
    ScrollablePanel();
    
    /**
     * @brief Set scroll mode
     */
    void setScrollMode(ScrollMode mode);
    
    /**
     * @brief Get scroll mode
     */
    ScrollMode getScrollMode() const { return scrollMode_; }
    
    /**
     * @brief Set scroll bar visibility
     */
    void setScrollBarVisibility(ScrollBarVisibility visibility);
    
    /**
     * @brief Get scroll bar visibility
     */
    ScrollBarVisibility getScrollBarVisibility() const { return scrollBarVisibility_; }
    
    /**
     * @brief Set scroll position
     */
    void setScrollPosition(float x, float y);
    
    /**
     * @brief Get scroll position
     */
    Vec2f getScrollPosition() const { return Vec2f(scrollX_, scrollY_); }
    
    /**
     * @brief Set scroll offset
     */
    void setScrollOffset(float dx, float dy);
    
    /**
     * @brief Set scroll speed
     */
    void setScrollSpeed(float speed);
    
    /**
     * @brief Get scroll speed
     */
    float getScrollSpeed() const { return scrollSpeed_; }
    
    /**
     * @brief Scroll to top
     */
    void scrollToTop();
    
    /**
     * @brief Scroll to bottom
     */
    void scrollToBottom();
    
    /**
     * @brief Scroll to left
     */
    void scrollToLeft();
    
    /**
     * @brief Scroll to right
     */
    void scrollToRight();
    
    /**
     * @brief Scroll to position
     */
    void scrollTo(float x, float y);
    
    /**
     * @brief Check if scrollable horizontally
     */
    bool isScrollableH() const { return scrollMode_ == ScrollMode::VerticalAndHorizontal ||
        scrollMode_ == ScrollMode::Horizontal; }
    
    /**
     * @brief Check if scrollable vertically
     */
    bool isScrollableV() const { return scrollMode_ == ScrollMode::VerticalAndHorizontal ||
        scrollMode_ == ScrollMode::Vertical; }
    
    /**
     * @brief Get content size
     */
    Vec2f getContentSize() const override;
    
    /**
     * @brief Handle mouse wheel
     */
    bool handleMouseWheel(float delta) override;
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
    void onLayoutChildren() override;
    
private:
    ScrollMode scrollMode_ = ScrollMode::Vertical;
    ScrollBarVisibility scrollBarVisibility_ = ScrollBarVisibility::Auto;
    float scrollX_ = 0.0f;
    float scrollY_ = 0.0f;
    float scrollSpeed_ = 1.0f;
    
    std::shared_ptr<class ScrollBar> hScrollBar_;
    std::shared_ptr<class ScrollBar> vScrollBar_;
    
    Vec2f contentSize_ = Vec2f::ZERO;
    bool contentSizeValid_ = false;
};

} // namespace vgui
