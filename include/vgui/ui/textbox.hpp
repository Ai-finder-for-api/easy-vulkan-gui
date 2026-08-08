#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Textbox                                                ║
// ║  Single-line editable text input.                               ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto& tb  = vgui::make_textbox("Enter name...");             ║
// ║    tb.on_submit = [&](auto& s){ login(s); };                    ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "widget.hpp"
#include <string>
#include <memory>
#include <functional>

namespace vgui {

class Textbox : public Widget {
public:
    std::string  text;
    std::string  placeholder;
    Color        text_color        = Color::white();
    Color        placeholder_color = Color::gray();
    Color        cursor_color      = Color::white();
    float        font_size         = 14.0f;
    size_t       max_length        = 256;
    bool         password_mode     = false;   ///< Shows *** instead of text
    bool         read_only         = false;
    bool         focused           = false;

    std::function<void(const std::string&)> on_change;
    std::function<void(const std::string&)> on_submit;   ///< Enter pressed

    static std::shared_ptr<Textbox> create(const std::string& placeholder = "");

    void update(float dt) override;
    void draw()           override;

    void focus();
    void blur();

private:
    Textbox() = default;
    size_t  m_cursor   = 0;
    size_t  m_sel_start= 0;
    float   m_blink_t  = 0;
};

} // namespace vgui
