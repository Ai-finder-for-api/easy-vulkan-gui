#include <vgui/ui/label.hpp>

namespace vgui {

std::shared_ptr<Label> Label::create(const std::string& t) {
    auto l = std::shared_ptr<Label>(new Label);
    l->text = t; return l;
}

void Label::draw() {
    if (!visible) return;
    // Delegate to Canvas immediate-mode text draw
    // Canvas::active().draw_text(text, x, y, color, font_size, font.get());
    for (auto& c : m_children) c->draw();
}

} // namespace vgui
