// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Gamepad  –  implementation  (platform stubs)           ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/input/gamepad.hpp>
#include <cstring>

#if defined(_WIN32)
#  include <windows.h>
#  include <xinput.h>
#endif

namespace vgui {

Gamepad& Gamepad::get() { static Gamepad s; return s; }

const GamepadState& Gamepad::state(int i)     const { return m_state[i]; }
bool                Gamepad::is_connected(int i) const { return m_state[i].connected; }

bool Gamepad::is_down(GamepadButton btn, int idx) const {
    return m_state[idx].buttons[static_cast<int>(btn)];
}
bool Gamepad::pressed(GamepadButton btn, int idx) const {
    int i = static_cast<int>(btn);
    return m_state[idx].buttons[i] && !m_prev[idx].buttons[i];
}
bool Gamepad::released(GamepadButton btn, int idx) const {
    int i = static_cast<int>(btn);
    return !m_state[idx].buttons[i] && m_prev[idx].buttons[i];
}

static float apply_dead_zone(float v, float dz) {
    if (v >  dz) return (v - dz) / (1.0f - dz);
    if (v < -dz) return (v + dz) / (1.0f - dz);
    return 0.0f;
}

void Gamepad::_poll() {
    std::memcpy(m_prev, m_state, sizeof(m_state));

#if defined(_WIN32)
    for (int i = 0; i < MAX_PADS; ++i) {
        XINPUT_STATE xs{};
        if (XInputGetState(i, &xs) != ERROR_SUCCESS) {
            if (m_state[i].connected && on_connection) on_connection(i, false);
            m_state[i] = {};
            continue;
        }
        if (!m_state[i].connected && on_connection) on_connection(i, true);
        m_state[i].connected = true;

        auto& g = m_state[i];
        auto& p = xs.Gamepad;

        g.left_stick.x  = apply_dead_zone(p.sThumbLX / 32767.f, dead_zone);
        g.left_stick.y  = apply_dead_zone(p.sThumbLY / 32767.f, dead_zone);
        g.right_stick.x = apply_dead_zone(p.sThumbRX / 32767.f, dead_zone);
        g.right_stick.y = apply_dead_zone(p.sThumbRY / 32767.f, dead_zone);
        g.left_trigger  = p.bLeftTrigger  / 255.f;
        g.right_trigger = p.bRightTrigger / 255.f;

        auto btn = [&](WORD mask, GamepadButton b) {
            g.buttons[static_cast<int>(b)] = (p.wButtons & mask) != 0;
        };
        btn(XINPUT_GAMEPAD_A,              GamepadButton::A);
        btn(XINPUT_GAMEPAD_B,              GamepadButton::B);
        btn(XINPUT_GAMEPAD_X,              GamepadButton::X);
        btn(XINPUT_GAMEPAD_Y,              GamepadButton::Y);
        btn(XINPUT_GAMEPAD_LEFT_SHOULDER,  GamepadButton::LeftBumper);
        btn(XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadButton::RightBumper);
        btn(XINPUT_GAMEPAD_BACK,           GamepadButton::Back);
        btn(XINPUT_GAMEPAD_START,          GamepadButton::Start);
        btn(XINPUT_GAMEPAD_LEFT_THUMB,     GamepadButton::LeftThumb);
        btn(XINPUT_GAMEPAD_RIGHT_THUMB,    GamepadButton::RightThumb);
        btn(XINPUT_GAMEPAD_DPAD_UP,        GamepadButton::DpadUp);
        btn(XINPUT_GAMEPAD_DPAD_RIGHT,     GamepadButton::DpadRight);
        btn(XINPUT_GAMEPAD_DPAD_DOWN,      GamepadButton::DpadDown);
        btn(XINPUT_GAMEPAD_DPAD_LEFT,      GamepadButton::DpadLeft);
    }
#endif
    // Linux / macOS: evdev / IOKit gamepad polling would go here
}

void Gamepad::_end_frame() { /* pressed/released derived per-query from prev */ }

} // namespace vgui
