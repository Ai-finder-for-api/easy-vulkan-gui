#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Label                                                  ║
// ║  Static text element.                                           ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto& lbl   = vgui::make_label("Hello, world!");             ║
// ║    lbl.color   = vgui::Color::white();                          ║
// ║    lbl.font_size = 18.0f;                                       ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "widget.hpp"
#include "font.hpp"
#include <string>
#include <memory>

namespace vgui {

enum class TextAlign : uint8_t { Left, Center, Right };
enum class TextWrap  : uint8_t { None, Word, Char };

class Label : public Widget {
public:
    std::string  text;
    Color        color      = Color::white();
    float        font_size  = 14.0f;
    TextAlign    align      = TextAlign::Left;
    TextWrap     wrap       = TextWrap::Word;
    bool         shadow     = false;
    Color        shadow_color = Color::black().with_alpha(0.5f);
    Vec2         shadow_offset = {1, 1};

    std::shared_ptr<Font> font;   ///< nullptr = built-in font

    static std::shared_ptr<Label> create(const std::string& text = "");

    void draw() override;

private:
    Label() = default;
};

} // namespace vgui
