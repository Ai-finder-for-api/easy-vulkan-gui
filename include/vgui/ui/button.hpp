#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Button                                                 ║
// ║  Clickable button with label and hover/press states.            ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto& btn    = vgui::make_button("Click me");                ║
// ║    btn.width    = 120;                                          ║
// ║    btn.height   = 36;                                           ║
// ║    btn.on_click = [] { VGUI_INFO("clicked!"); };                ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "widget.hpp"
#include <string>
#include <memory>

namespace vgui {

class Button : public Widget {
public:
    std::string  text;
    Color  normal_color  = Color::from_hex(0x3A3A3AFF);
    Color  hover_color   = Color::from_hex(0x505050FF);
    Color  press_color   = Color::from_hex(0x222222FF);
    Color  text_color    = Color::white();
    float  font_size     = 14.0f;
    float  corner_radius = 4.0f;

    bool   is_hovered = false;
    bool   is_pressed = false;

    static std::shared_ptr<Button> create(const std::string& text = "Button");

    void update(float dt) override;
    void draw()           override;

private:
    Button() = default;
};

} // namespace vgui
