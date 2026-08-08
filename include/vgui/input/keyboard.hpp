#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Keyboard                                               ║
// ║  Platform-agnostic keyboard state.                              ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    if (vgui::Keyboard::pressed(vgui::Key::Space)) { ... }       ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <array>
#include <functional>

namespace vgui {

/// Platform-agnostic key codes
enum class Key : uint16_t {
    Unknown = 0,
    // -- Printable
    Space, Apostrophe, Comma, Minus, Period, Slash,
    D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,
    Semicolon, Equal,
    A,B,C,D,E,F,G,H,I,J,K,L,M,
    N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
    LeftBracket, Backslash, RightBracket, GraveAccent,
    // -- Function
    Escape, Enter, Tab, Backspace, Insert, Delete,
    Right, Left, Down, Up,
    PageUp, PageDown, Home, End,
    CapsLock, ScrollLock, NumLock, PrintScreen, Pause,
    F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
    // -- Modifiers
    LeftShift, LeftCtrl, LeftAlt, LeftSuper,
    RightShift, RightCtrl, RightAlt, RightSuper,
    Menu,
    // -- Numpad
    Num0,Num1,Num2,Num3,Num4,Num5,Num6,Num7,Num8,Num9,
    NumDecimal, NumDivide, NumMultiply, NumSubtract, NumAdd, NumEnter,
    _Count
};

enum class KeyMod : uint8_t {
    None  = 0,
    Shift = 1 << 0,
    Ctrl  = 1 << 1,
    Alt   = 1 << 2,
    Super = 1 << 3
};
inline KeyMod operator|(KeyMod a, KeyMod b) { return static_cast<KeyMod>(uint8_t(a)|uint8_t(b)); }
inline bool   operator&(KeyMod a, KeyMod b) { return (uint8_t(a)&uint8_t(b)) != 0; }

class Keyboard {
public:
    static Keyboard& get();

    /// True the entire time the key is held
    bool is_down   (Key k) const;
    /// True only on the first frame the key was pressed
    bool pressed   (Key k) const;
    /// True only on the frame the key was released
    bool released  (Key k) const;

    KeyMod modifiers() const;

    /// Callback fires every time a Unicode character is typed
    std::function<void(uint32_t codepoint)> on_char;

    /// Raw callback: key, scancode, pressed/released
    std::function<void(Key, int scancode, bool down, KeyMod)> on_key;

    // ── Internal: called by the platform layer ────────────
    void _notify_key (Key k, int sc, bool down, KeyMod mods);
    void _notify_char(uint32_t cp);
    void _end_frame  ();     ///< Clear single-frame pressed/released flags

private:
    Keyboard() = default;
    static constexpr size_t KEY_COUNT = static_cast<size_t>(Key::_Count);
    std::array<bool, KEY_COUNT> m_down     {};
    std::array<bool, KEY_COUNT> m_pressed  {};
    std::array<bool, KEY_COUNT> m_released {};
    KeyMod                      m_mods     = KeyMod::None;
};

} // namespace vgui
