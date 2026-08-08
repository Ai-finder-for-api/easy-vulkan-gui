#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Widget                                                 ║
// ║  Base class for all UI elements.                                ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec2.hpp"
#include "../math/color.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace vgui {

enum class Anchor : uint8_t {
    TopLeft, TopCenter, TopRight,
    MiddleLeft, Center, MiddleRight,
    BottomLeft, BottomCenter, BottomRight
};

class Widget : public std::enable_shared_from_this<Widget> {
public:
    std::string name;

    // ── Layout (easy-assign syntax) ───────────────────────
    float  x        = 0;
    float  y        = 0;
    float  width    = 100;
    float  height   = 30;
    Anchor anchor   = Anchor::TopLeft;
    float  padding  = 0;
    float  opacity  = 1.0f;
    bool   visible  = true;
    bool   enabled  = true;

    // ── Background ────────────────────────────────────────
    Color  bg_color         = Color::transparent();
    float  corner_radius    = 0.0f;
    float  border_width     = 0.0f;
    Color  border_color     = Color::gray();

    // ── Children ──────────────────────────────────────────
    void add_child   (std::shared_ptr<Widget> child);
    void remove_child(std::shared_ptr<Widget> child);
    const std::vector<std::shared_ptr<Widget>>& children() const;

    // ── Lifecycle ─────────────────────────────────────────
    virtual void update(float dt) {}
    virtual void draw  () {}

    // ── Hit testing ───────────────────────────────────────
    bool contains(Vec2 screen_pos) const;

    // ── Callbacks ─────────────────────────────────────────
    std::function<void()>          on_click;
    std::function<void()>          on_hover_enter;
    std::function<void()>          on_hover_exit;
    std::function<void(Vec2 pos)>  on_drag;

protected:
    Widget() = default;
    Widget*                             m_parent = nullptr;
    std::vector<std::shared_ptr<Widget>> m_children;
};

} // namespace vgui
