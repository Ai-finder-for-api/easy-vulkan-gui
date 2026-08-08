// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Mouse  –  implementation                               ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/input/mouse.hpp>

namespace vgui {

Mouse& Mouse::get() { static Mouse s; return s; }

Vec2 Mouse::position() const { return m_pos; }
Vec2 Mouse::delta()    const { return m_pos - m_prev_pos; }
Vec2 Mouse::scroll()   const { return m_scroll; }

bool Mouse::is_down (MouseButton b) const { return m_down    [static_cast<int>(b)]; }
bool Mouse::pressed (MouseButton b) const { return m_pressed [static_cast<int>(b)]; }
bool Mouse::released(MouseButton b) const { return m_released[static_cast<int>(b)]; }

void Mouse::set_mode(CursorMode mode) { m_mode = mode; /* platform sets actual cursor */ }
CursorMode Mouse::mode() const { return m_mode; }

void Mouse::_notify_move(Vec2 pos) {
    m_prev_pos = m_pos; m_pos = pos;
    if (on_move) on_move(pos);
}

void Mouse::_notify_button(MouseButton btn, bool down) {
    int i = static_cast<int>(btn);
    if (down) { m_pressed[i]  = !m_down[i]; m_down[i] = true;  }
    else       { m_released[i] = m_down[i];  m_down[i] = false; }
    if (on_button) on_button(btn, down);
}

void Mouse::_notify_scroll(Vec2 delta) {
    m_scroll = delta;
    if (on_scroll) on_scroll(delta);
}

void Mouse::_end_frame() {
    m_prev_pos = m_pos;
    m_scroll   = {};
    for (int i = 0; i < static_cast<int>(MouseButton::_Count); ++i)
        m_pressed[i] = m_released[i] = false;
}

} // namespace vgui
