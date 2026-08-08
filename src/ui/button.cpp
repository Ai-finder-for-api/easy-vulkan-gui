#include <vgui/ui/button.hpp>
#include <vgui/input/mouse.hpp>

namespace vgui {

std::shared_ptr<Button> Button::create(const std::string& t) {
    auto b = std::shared_ptr<Button>(new Button);
    b->text = t; return b;
}

void Button::update(float /*dt*/) {
    if (!visible || !enabled) return;
    Vec2 mp = Mouse::get().position();
    bool over = contains(mp);
    bool click = Mouse::get().pressed(MouseButton::Left);

    if (over && !is_hovered) { is_hovered = true; if (on_hover_enter) on_hover_enter(); }
    if (!over && is_hovered) { is_hovered = false; if (on_hover_exit)  on_hover_exit();  }

    is_pressed = over && Mouse::get().is_down(MouseButton::Left);
    if (over && click && on_click) on_click();
}

void Button::draw() {
    if (!visible) return;
    Color c = is_pressed ? press_color : is_hovered ? hover_color : normal_color;
    // Canvas::active().draw_rect(x, y, width, height, c, corner_radius);
    // Canvas::active().draw_text(text, x + width/2, y + height/2, text_color, font_size);
    for (auto& ch : m_children) ch->draw();
}

} // namespace vgui
