#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Mouse                                                  ║
// ║  Platform-agnostic mouse / pointer state.                       ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec2.hpp"
#include <functional>
#include <cstdint>

namespace vgui {

enum class MouseButton : uint8_t {
    Left   = 0,
    Right  = 1,
    Middle = 2,
    X1     = 3,
    X2     = 4,
    _Count = 5
};

enum class CursorMode : uint8_t {
    Normal,     ///< Visible, free to leave window
    Hidden,     ///< Invisible but still free
    Captured    ///< Invisible and locked to window (FPS-style)
};

class Mouse {
public:
    static Mouse& get();

    /// Cursor position in screen pixels (top-left origin)
    Vec2  position()  const;
    /// Delta since last frame
    Vec2  delta()     const;
    /// Scroll wheel delta (y = vertical)
    Vec2  scroll()    const;

    bool  is_down (MouseButton btn) const;
    bool  pressed (MouseButton btn) const;
    bool  released(MouseButton btn) const;

    void  set_mode(CursorMode mode);
    CursorMode mode() const;

    /// Callbacks
    std::function<void(Vec2 pos)>              on_move;
    std::function<void(MouseButton, bool down)> on_button;
    std::function<void(Vec2 delta)>            on_scroll;

    // ── Internal ──────────────────────────────────────────
    void _notify_move  (Vec2 pos);
    void _notify_button(MouseButton btn, bool down);
    void _notify_scroll(Vec2 delta);
    void _end_frame    ();

private:
    Mouse() = default;
    Vec2 m_pos{}, m_prev_pos{}, m_scroll{};
    bool m_down    [static_cast<int>(MouseButton::_Count)]{};
    bool m_pressed [static_cast<int>(MouseButton::_Count)]{};
    bool m_released[static_cast<int>(MouseButton::_Count)]{};
    CursorMode m_mode = CursorMode::Normal;
};

} // namespace vgui
