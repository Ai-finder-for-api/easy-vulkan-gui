#pragma once

#include <string>
#include <memory>

#include "vgui/ui/widget.hpp"

namespace vgui {

/**
 * @brief Text alignment
 */
enum class TextAlignment {
    Left = 0,
    Center,
    Right
};

/**
 * @brief Text overflow mode
 */
enum class TextOverflow {
    Clip,        // Clip text that doesn't fit
    Ellipsis,    // Show "..." at end
    Resize,      // Resize widget to fit
    Scroll       // Enable scrolling
};

/**
 * @brief Label widget
 * 
 * Displays text with optional styling.
 */
class Label : public Widget {
public:
    /**
     * @brief Create a label
     */
    Label();
    
    /**
     * @brief Create label with text
     */
    Label(const std::string& text);
    
    /**
     * @brief Create label with text and size
     */
    Label(const std::string& text, float width, float height);
    
    /**
     * @brief Set text
     */
    void setText(const std::string& text) override;
    
    /**
     * @brief Get text
     */
    std::string getText() const override { return text_; }
    
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
     * @brief Set font
     */
    void setFont(std::shared_ptr<class Font> font);
    
    /**
     * @brief Get font
     */
    std::shared_ptr<class Font> getFont() const { return font_; }
    
    /**
     * @brief Set font size
     */
    void setFontSize(float size);
    
    /**
     * @brief Get font size
     */
    float getFontSize() const { return fontSize_; }
    
    /**
     * @brief Set line spacing
     */
    void setLineSpacing(float spacing);
    
    /**
     * @brief Get line spacing
     */
    float getLineSpacing() const { return lineSpacing_; }
    
    /**
     * @brief Set overflow mode
     */
    void setOverflowMode(TextOverflow mode);
    
    /**
     * @brief Get overflow mode
     */
    TextOverflow getOverflowMode() const { return overflowMode_; }
    
    /**
     * @brief Check if text is truncated
     */
    bool isTruncated() const { return truncated_; }
    
    /**
     * @brief Set shadow
     */
    void setShadow(bool enabled, const Vec2f& offset = Vec2f(1.0f, 1.0f),
                   const Colorf& color = Colorf(0.0f, 0.0f, 0.0f, 0.5f));
    
    /**
     * @brief Check if shadow is enabled
     */
    bool hasShadow() const { return shadowEnabled_; }
    
    /**
     * @brief Get desired size
     */
    Vec2f getDesiredSize() const override;
    
    /**
     * @brief Get content width
     */
    float getContentWidth() const override;
    
    /**
     * @brief Get content height
     */
    float getContentHeight() const override;
    
    /**
     * @brief Get text bounds
     */
    Rectf getTextBounds() const;
    
    /**
     * @brief Get line count
     */
    uint32_t getLineCount() const { return lineCount_; }
    
    /**
     * @brief Get wrapped lines
     */
    const std::vector<std::string>& getLines() const { return lines_; }
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
    virtual void onTextChanged();
    
private:
    std::shared_ptr<class Font> font_;
    float fontSize_ = 14.0f;
    TextAlignment textAlignment_ = TextAlignment::Left;
    Colorf textColor_ = Colorf(1.0f, 1.0f, 1.0f);
    float lineSpacing_ = 1.2f;
    TextOverflow overflowMode_ = TextOverflow::Clip;
    
    bool truncated_ = false;
    uint32_t lineCount_ = 1;
    std::vector<std::string> lines_;
    Vec2f textBounds_ = Vec2f::ZERO;
    
    // Shadow
    bool shadowEnabled_ = false;
    Vec2f shadowOffset_ = Vec2f(1.0f, 1.0f);
    Colorf shadowColor_ = Colorf(0.0f, 0.0f, 0.0f, 0.5f);
    
    // Layout cache
    Vec2f desiredSizeCache_ = Vec2f::ZERO;
    bool desiredSizeCacheValid_ = false;
    
    void updateLayout();
    void wrapText(const std::string& text, float maxWidth);
    float measureTextWidth(const std::string& text) const;
    float measureTextHeight() const;
    
    friend class Canvas;
};

/**
 * @brief Rich text label with multiple styles
 */
class RichLabel : public Label {
public:
    /**
     * @brief Create rich label
     */
    RichLabel();
    
    /**
     * @brief Add text segment with style
     */
    void addText(const std::string& text,
                 const Colorf& color = Colorf::WHITE,
                 float fontSize = 0.0f,  // 0 = default
                 bool bold = false,
                 bool italic = false);
    
    /**
     * @brief Clear all text
     */
    void clear();
    
    /**
     * @brief Get text segments
     */
    const std::vector<TextSegment>& getSegments() const { return segments_; }
    
    /**
     * @brief Set text segments
     */
    void setSegments(const std::vector<TextSegment>& segments);
    
    /**
     * @brief Get desired size
     */
    Vec2f getDesiredSize() const override;
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
private:
    struct TextSegment {
        std::string text;
        Colorf color;
        float fontSize;
        bool bold;
        bool italic;
    };
    
    std::vector<TextSegment> segments_;
    
    void updateLayout() override;
};

/**
 * @brief Bold label
 */
class BoldLabel : public Label {
public:
    BoldLabel() {
        // Will be implemented with bold font support
    }
};

/**
 * @brief Icon label (icon + text)
 */
class IconLabel : public Label {
public:
    /**
     * @brief Create icon label
     */
    IconLabel();
    
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
     * @brief Set icon alignment
     */
    void setIconAlignment(TextAlignment alignment);
    
    /**
     * @brief Get icon alignment
     */
    TextAlignment getIconAlignment() const { return iconAlignment_; }
    
    /**
     * @brief Get desired size
     */
    Vec2f getDesiredSize() const override;
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
private:
    std::shared_ptr<class Texture> icon_;
    float iconSize_ = 16.0f;
    float iconSpacing_ = 6.0f;
    TextAlignment iconAlignment_ = TextAlignment::Left;
    
    Vec2f desiredSizeCache_ = Vec2f::ZERO;
    bool desiredSizeCacheValid_ = false;
    
    void updateLayout();
};

} // namespace vgui
