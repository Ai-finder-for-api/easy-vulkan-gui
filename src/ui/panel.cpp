#include <vgui/ui/panel.hpp>

namespace vgui {

std::shared_ptr<Panel> Panel::create() {
    return std::shared_ptr<Panel>(new Panel);
}

void Panel::update(float dt) {
    if (!visible) return;
    for (auto& c : m_children) c->update(dt);
}

void Panel::draw() {
    if (!visible) return;
    // Canvas::active().draw_rect(x, y, width, height, bg_color, corner_radius);
    for (auto& c : m_children) c->draw();
}

} // namespace vgui
