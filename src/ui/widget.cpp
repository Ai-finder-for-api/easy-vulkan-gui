#include <vgui/ui/widget.hpp>
#include <algorithm>

namespace vgui {

void Widget::add_child(std::shared_ptr<Widget> child) {
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

void Widget::remove_child(std::shared_ptr<Widget> child) {
    m_children.erase(std::remove(m_children.begin(), m_children.end(), child), m_children.end());
    child->m_parent = nullptr;
}

const std::vector<std::shared_ptr<Widget>>& Widget::children() const { return m_children; }

bool Widget::contains(Vec2 p) const {
    return p.x >= x && p.x <= x + width && p.y >= y && p.y <= y + height;
}

} // namespace vgui
