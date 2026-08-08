#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Gamepad                                                ║
// ║  Xbox-style controller input. Up to 4 controllers.             ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec2.hpp"
#include <cstdint>
#include <functional>

namespace vgui {

enum class GamepadButton : uint16_t {
    A=0, B, X, Y,
    LeftBumper, RightBumper,
    Back, Start, Guide,
    LeftThumb, RightThumb,
    DpadUp, DpadRight, DpadDown, DpadLeft,
    _Count
};

struct GamepadState {
    bool  buttons[static_cast<int>(GamepadButton::_Count)]{};
    Vec2  left_stick{};       ///< -1..+1
    Vec2  right_stick{};      ///< -1..+1
    float left_trigger  = 0;  ///<  0..+1
    float right_trigger = 0;  ///<  0..+1
    bool  connected     = false;
    char  name[64]      = {};
};

class Gamepad {
public:
    static Gamepad& get();

    static constexpr int MAX_PADS = 4;

    const GamepadState& state(int index = 0) const;
    bool is_connected(int index = 0)         const;

    bool pressed (GamepadButton btn, int index = 0) const;
    bool released(GamepadButton btn, int index = 0) const;
    bool is_down (GamepadButton btn, int index = 0) const;

    float dead_zone = 0.1f;  ///< Stick dead-zone radius

    std::function<void(int index, bool connected)> on_connection;

    void _poll();
    void _end_frame();

private:
    Gamepad() = default;
    GamepadState m_state[MAX_PADS]{};
    GamepadState m_prev [MAX_PADS]{};
};

} // namespace vgui
