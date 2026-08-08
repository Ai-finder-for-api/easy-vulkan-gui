#include <vgui/ui/layout.hpp>

namespace vgui {

std::shared_ptr<Layout> Layout::create(LayoutType t) {
    auto l = std::shared_ptr<Layout>(new Layout);
    l->type = t; return l;
}

void Layout::arrange() {
    float cx = x + pad_x, cy = y + pad_y;
    int   col = 0;
    for (auto& child : m_children) {
        if (!child->visible) continue;
        switch (type) {
            case LayoutType::VStack:
                child->x = cx; child->y = cy;
                cy += child->height + gap;
                break;
            case LayoutType::HStack:
                child->x = cx; child->y = cy;
                cx += child->width + gap;
                break;
            case LayoutType::Grid:
                child->x = cx; child->y = cy;
                if (++col >= columns) {
                    col = 0; cx = x + pad_x;
                    cy += child->height + gap;
                } else {
                    cx += child->width + gap;
                }
                break;
            case LayoutType::Flex:
                child->x = cx; child->y = cy;
                cx += child->width + gap;
                if (cx + child->width > x + width) {
                    cx = x + pad_x; cy += child->height + gap;
                }
                break;
            default: break;
        }
    }
}

void Layout::update(float dt) { arrange(); for (auto& c : m_children) c->update(dt); }
void Layout::draw()           { arrange(); for (auto& c : m_children) c->draw(); }

} // namespace vgui
