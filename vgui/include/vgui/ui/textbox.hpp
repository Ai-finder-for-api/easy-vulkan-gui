#pragma once

#include <string>
#include <vector>
#include <memory>

#include "vgui/ui/widget.hpp"
#include "vgui/ui/font.hpp"

namespace vgui {

/**
 * @brief Text input mode
 */
enum class InputMode {
    Text,        // Normal text input
    Password,    // Password (shows dots)
    Number,      // Numeric input only
    Email,       // Email input
    URL,         // URL input
    Multiline    // Multiline text
};

/**
 * @brief Text alignment for textbox content
 */
enum class TextBoxAlignment {
    Left = 0,
    Center,
    Right
};

/**
 * @brief TextBox widget
 * 
 * A text input field for user text input.
 */
class TextBox : public Widget {
public:
    /**
     * @brief Create a textbox
     */
    TextBox();
    
    /**
     * @brief Create textbox with placeholder
     */
    TextBox(const std::string& placeholder);
    
    /**
     * @brief Set text
     */
    void setText(const std::string& text) override;
    
    /**
     * @brief Get text
     */
    std::string getText() const override { return text_; }
    
    /**
     * @brief Set placeholder text
     */
    void setPlaceholder(const std::string& placeholder);
    
    /**
     * @brief Get placeholder
     */
    std::string getPlaceholder() const { return placeholder_; }
    
    /**
     * @brief Set input mode
     */
    void setInputMode(InputMode mode);
    
    /**
     * @brief Get input mode
     */
    InputMode getInputMode() const { return inputMode_; }
    
    /**
     * @brief Set text alignment
     */
    void setAlignment(TextBoxAlignment alignment);
    
    /**
     * @brief Get text alignment
     */
    TextBoxAlignment getAlignment() const { return alignment_; }
    
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
     * @brief Set text color
     */
    void setTextColor(const Colorf& color);
    
    /**
     * @brief Get text color
     */
    Colorf getTextColor() const { return textColor_; }
    
    /**
     * @brief Set placeholder color
     */
    void setPlaceholderColor(const Colorf& color);
    
    /**
     * @brief Get placeholder color
     */
    Colorf getPlaceholderColor() const { return placeholderColor_; }
    
    /**
     * @brief Set caret color
     */
    void setCaretColor(const Colorf& color);
    
    /**
     * @brief Get caret color
     */
    Colorf getCaretColor() const { return caretColor_; }
    
    /**
     * @brief Set selection color
     */
    void setSelectionColor(const Colorf& color);
    
    /**
     * @brief Get selection color
     */
    Colorf getSelectionColor() const { return selectionColor_; }
    
    /**
     * @brief Get text bounds (for scrolling)
     */
    Rectf getTextBounds() const;
    
    /**
     * @brief Get caret position (visual)
     */
    float getCaretPosition() const;
    
    /**
     * @brief Set caret position
     */
    void setCaretPosition(size_t position);
    
    /**
     * @brief Get selection start
     */
    size_t getSelectionStart() const { return selectionStart_; }
    
    /**
     * @brief Get selection end
     */
    size_t getSelectionEnd() const { return selectionEnd_; }
    
    /**
     * @brief Get selection length
     */
    size_t getSelectionLength() const;
    
    /**
     * @brief Select all text
     */
    void selectAll();
    
    /**
     * @brief Clear selection
     */
    void clearSelection();
    
    /**
     * @brief Replace selected text
     */
    void replaceSelection(const std::string& text);
    
    /**
     * @brief Insert text at caret
     */
    void insertText(const std::string& text);
    
    /**
     * @brief Delete character before caret
     */
    void deleteBeforeCaret();
    
    /**
     * @brief Delete character after caret
     */
    void deleteAfterCaret();
    
    /**
     * @brief Move caret left
     */
    void moveCaretLeft(bool extend);
    
    /**
     * @brief Move caret right
     */
    void moveCaretRight(bool extend);
    
    /**
     * @brief Move caret to start
     */
    void moveCaretToStart(bool extend);
    
    /**
     * @brief Move caret to end
     */
    void moveCaretToEnd(bool extend);
    
    /**
     * @brief Move caret up (multiline)
     */
    void moveCaretUp(bool extend);
    
    /**
     * @brief Move caret down (multiline)
     */
    void moveCaretDown(bool extend);
    
    /**
     * @brief Check if text is empty
     */
    bool isEmpty() const { return text_.empty(); }
    
    /**
     * @brief Set max length
     */
    void setMaxLength(size_t maxLength);
    
    /**
     * @brief Get max length
     */
    size_t getMaxLength() const { return maxLength_; }
    
    /**
     * @brief Check if text is read-only
     */
    bool isReadOnly() const { return readOnly_; }
    
    /**
     * @brief Set read-only
     */
    void setReadOnly(bool readOnly);
    
    /**
     * @brief Check if textbox is multiline
     */
    bool isMultiline() const { return inputMode_ == InputMode::Multiline; }
    
    /**
     * @brief Set multiline
     */
    void setMultiline(bool multiline);
    
    /**
     * @brief Set scroll position
     */
    void setScrollX(float scroll);
    
    /**
     * @brief Get scroll position
     */
    float getScrollX() const { return scrollX_; }
    
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
    
    /**
     * @brief Handle mouse move
     */
    bool handleMouseMove(float x, float y) override;
    
    /**
     * @brief Handle mouse wheel
     */
    bool handleMouseWheel(float delta) override;
    
    /**
     * @brief Handle key down
     */
    bool handleKeyDown(uint32_t key, uint32_t mods) override;
    
    /**
     * @brief Handle char input
     */
    bool handleCharInput(uint32_t character) override;
    
    /**
     * @brief Render textbox
     */
    void render(class CommandBuffer& cmdBuffer,
               const Mat4f& transform,
               const Colorf& colorOverride = Colorf::WHITE) override;
    
    /**
     * @brief Set text changed callback
     */
    void onTextChanged(WidgetEventCallback callback);
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
    void onFocus(bool focused) override;
    void onHover(bool hovered) override;
    void onActivate(bool activated) override;
    
    void updateCaretPosition();
    void updateScroll();
    void updateTextBounds();
    
    std::shared_ptr<class Font> font_;
    float fontSize_ = 14.0f;
    TextBoxAlignment alignment_ = TextBoxAlignment::Left;
    
    Colorf textColor_ = Colorf(1.0f, 1.0f, 1.0f);
    Colorf placeholderColor_ = Colorf(0.4f, 0.4f, 0.4f, 0.7f);
    Colorf caretColor_ = Colorf(0.5f, 0.5f, 0.5f);
    Colorf selectionColor_ = Colorf(0.3f, 0.3f, 0.3f, 0.5f);
    
    std::string placeholder_;
    InputMode inputMode_ = InputMode::Text;
    bool readOnly_ = false;
    size_t maxLength_ = 0;  // 0 = unlimited
    
    std::string text_;
    size_t caretPosition_ = 0;
    size_t selectionStart_ = 0;
    size_t selectionEnd_ = 0;
    
    float scrollX_ = 0.0f;
    float scrollY_ = 0.0f;
    
    Rectf textBounds_ = Rectf::ZERO;
    Vec2f textSize_ = Vec2f::ZERO;
    float textWidthCache_ = 0.0f;
    mutable float caretVisualPos_ = 0.0f;
    
    bool caretVisible_ = true;
    float caretBlinkTime_ = 0.0f;
    
    // Callback
    WidgetEventCallback textChangedCallback_;
    
    mutable bool textBoundsCacheValid_ = false;
    
    void fireTextChanged();
    bool isValidPosition(size_t pos) const;
    size_t getVisualPosition(size_t logicalPos) const;
    size_t getLogicalPosition(float visualPos) const;
    
    friend class Canvas;
};

/**
 * @brief Password textbox
 */
class PasswordBox : public TextBox {
public:
    /**
     * @brief Create password box
     */
    PasswordBox();
    
    /**
     * @brief Set password mask character
     */
    void setMaskChar(char mask);
    
    /**
     * @brief Get mask character
     */
    char getMaskChar() const { return maskChar_; }
    
    /**
     * @brief Get text (returns actual password)
     */
    std::string getText() const override { return text_; }
    
protected:
    void onRender(class CommandBuffer& cmdBuffer,
                 const Mat4f& transform,
                 const Colorf& colorOverride) override;
    
private:
    char maskChar_ = '*';
};

/**
 * @brief Number textbox
 */
class NumberBox : public TextBox {
public:
    /**
     * @brief Create number box
     */
    NumberBox();
    
    /**
     * @brief Set value
     */
    void setValue(double value);
    
    /**
     * @brief Get value
     */
    double getValue() const { return value_; }
    
    /**
     * @brief Set min value
     */
    void setMinValue(double min);
    
    /**
     * @brief Get min value
     */
    double getMinValue() const { return minValue_; }
    
    /**
     * @brief Set max value
     */
    void setMaxValue(double max);
    
    /**
     * @brief Get max value
     */
    double getMaxValue() const { return maxValue_; }
    
    /**
     * @brief Set step
     */
    void setStep(double step);
    
    /**
     * @brief Get step
     */
    double getStep() const { return step_; }
    
    /**
     * @brief Set value changed callback
     */
    void onValueChanged(std::function<void(double)> callback);
    
protected:
    bool handleKeyUp(uint32_t key, uint32_t mods) override;
    
private:
    double value_ = 0.0;
    double minValue_ = -1e100;
    double maxValue_ = 1e100;
    double step_ = 1.0;
    
    std::function<void(double)> valueChangedCallback_;
    
    void clampValue();
    void fireValueChanged();
    
    std::string formatValue(double v) const;
    bool parseValue(const std::string& s, double& out) const;
};

} // namespace vgui
