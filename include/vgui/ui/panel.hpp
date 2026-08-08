#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Panel                                                  ║
// ║  Rectangular container widget. Clip-masks its children.         ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "widget.hpp"
#include <memory>

namespace vgui {

class Panel : public Widget {
public:
    bool  clip_children = true;
    bool  scrollable    = false;
    Vec2  scroll_offset {};

    static std::shared_ptr<Panel> create();

    void update(float dt) override;
    void draw()           override;

private:
    Panel() = default;
};

} // namespace vgui
