#pragma once

#include <string>
#include <memory>

#include "vgui/ui/widget.hpp"

namespace vgui {

/**
 * @brief Button widget
 * 
 * A clickable button with optional text and icon.
 */
class Button : public Widget {
public:
    /**
     * @brief Create a button
     */
    Button();
    
    /**
     * @brief Create button with text
     */
    Button(const std::string& text);
    
    /**
     * @brief Create button with text and callback
     */
    Button(const std::string& text, WidgetEventCallback callback);
    
    /**
     * @brief Set text
     */
    void setText(const std::string& text) override;
    
    /**
     * @brief Get text
     */
    std::string getText() const override { return text_; }
    
    /**
     * @brief Set icon
     */
    void setIcon(std::shared_ptr<class Texture> icon);
    
    /**
     * @brief Get icon
     */
    std::shared_ptr<class Texture> getIcon() const { return icon_; }
    
    /**
     * @brief Set icon size
     */
    void setIconSize(float size);
    
    /**
     * @brief Get icon size
     */
    float getIconSize() const { return iconSize_; }
    
    /**
     * @brief Set icon spacing
     */
    void setIconSpacing(float spacing);
    
    /**
     * @brief Get icon spacing
     */
    float getIconSpacing() const { return iconSpacing_; }
    
    /**
     * @brief Set text alignment
     */
    void setTextAlignment(TextAlignment alignment);
    
    /**
     * @brief Get text alignment
     */
    TextAlignment getTextAlignment() const { return textAlignment_; }
    
    /**
     * @brief Set text color
     */
    void setTextColor(const Colorf& color);
    
    /**
     * @brief Get text color
     */
    Colorf getTextColor() const { return textColor_; }
    
    /**
     * @brief Set text size
     */
    void setTextSize(float size);
    
    /**
     * @brief Get text size
     */
    float getTextSize() const { return textSize_; }
    
    /**
     * @brief Set show focus ring
     */
    void setShowFocusRing(bool show);
    
    /**
     * @brief Get show focus ring
     */
    bool getShowFocusRing() const { return showFocusRing_; }
    
    /**
     * @brief Set corner rounding
     */
    void setCornerRounding(float radius) override;
    
    /**
     * @brief Get desired size
     */
    Vec2f getDesiredSize() const override;
    
    /**
     * @brief Get content size
     */
    Vec2f getContentSize() const;
    
    /**
     * @brief Handle mouse down
     */
    bool handleMouseDown(float x, float y, uint32_t button) override;
    
    /**
     * @brief Handle mouse up
     */
    bool handleMouseUp(float x, float y, uint32_t button) override;
    
    /**
     * @brief Handle mouse move
     */
    bool handleMouseMove(float x, float y) override;
    
    /**
     * @brief Render button
     */
    void render(class CommandBuffer& cmdBuffer,
               const Mat4f& transform,
               const Colorf& colorOverride = Colorf::WHITE) override;
    
    /**
     * @brief Set pressed state
     */
    void setPressed(bool pressed);
    
    /**
     * @brief Check if pressed
     */
    bool isPressed() const { return pressed_; }
    
protected:
    void onMouseDown(float x, float y, uint32_t button) override;
    void onMouseUp(float x, float y, uint32_t button) override;
    void onMouseMove(float x, float y) override;
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
    virtual void onClick() {
        fireEvent(WidgetEvent{WidgetEventType::Click, shared_from_this()});
    }
    
private:
    std::shared_ptr<class Texture> icon_;
    float iconSize_ = 16.0f;
    float iconSpacing_ = 8.0f;
    TextAlignment textAlignment_ = TextAlignment::Center;
    Colorf textColor_ = Colorf(1.0f, 1.0f, 1.0f);
    float textSize_ = 0.0f;  // 0 = use default
    bool showFocusRing_ = true;
    
    bool pressed_ = false;
    
    // Cached text layout
    Vec2f textSizeCache_ = Vec2f::ZERO;
    bool textSizeCacheValid_ = false;
    
    void updateTextSizeCache();
};

/**
 * @brief Toggle button
 */
class ToggleButton : public Button {
public:
    /**
     * @brief Create toggle button
     */
    ToggleButton();
    
    /**
     * @brief Create toggle button with text
     */
    ToggleButton(const std::string& text);
    
    /**
     * @brief Check if checked
     */
    bool isChecked() const { return checked_; }
    
    /**
     * @brief Set checked state
     */
    void setChecked(bool checked);
    
    /**
     * @brief Toggle state
     */
    void toggle();
    
    /**
     * @brief Set checked color
     */
    void setCheckedColor(const Colorf& color);
    
    /**
     * @brief Get checked color
     */
    Colorf getCheckedColor() const { return checkedColor_; }
    
    /**
     * @brief Set unchecked color
     */
    void setUncheckedColor(const Colorf& color);
    
    /**
     * @brief Get unchecked color
     */
    Colorf getUncheckedColor() const { return uncheckedColor_; }
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
private:
    bool checked_ = false;
    Colorf checkedColor_ = Colorf(0.2f, 0.6f, 0.9f);
    Colorf uncheckedColor_ = Colorf(0.25f, 0.25f, 0.3f);
};

/**
 * @brief Icon button (small button with just an icon)
 */
class IconButton : public Button {
public:
    /**
     * @brief Create icon button
     */
    IconButton();
    
    /**
     * @brief Create icon button with icon
     */
    IconButton(std::shared_ptr<class Texture> icon);
    
    /**
     * @brief Set icon
     */
    void setIcon(std::shared_ptr<class Texture> icon) override;
    
    /**
     * @brief Get desired size
     */
    Vec2f getDesiredSize() const override;
    
    /**
     * @brief Set button size
     */
    void setButtonSize(float size);
    
    /**
     * @brief Get button size
     */
    float getButtonSize() const { return buttonSize_; }
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
private:
    float buttonSize_ = 24.0f;
};

/**
 * @brief Text button (button with only text, no background)
 */
class TextButton : public Button {
public:
    /**
     * @brief Create text button
     */
    TextButton();
    
    /**
     * @brief Create text button with text
     */
    TextButton(const std::string& text);
    
    /**
     * @brief Set text color
     */
    void setTextColor(const Colorf& color) override;
    
    /**
     * @brief Get text color
     */
    Colorf getTextColor() const override { return textColor_; }
    
    /**
     * @brief Set hover text color
     */
    void setHoverTextColor(const Colorf& color);
    
    /**
     * @brief Get hover text color
     */
    Colorf getHoverTextColor() const { return hoverTextColor_; }
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
private:
    Colorf hoverTextColor_ = Colorf(0.5f, 0.5f, 0.5f);
};

} // namespace vgui
