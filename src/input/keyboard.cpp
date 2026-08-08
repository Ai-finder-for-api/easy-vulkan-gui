// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Keyboard  –  implementation                            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/input/keyboard.hpp>

namespace vgui {

Keyboard& Keyboard::get() { static Keyboard s; return s; }

bool Keyboard::is_down (Key k) const { return m_down    [static_cast<size_t>(k)]; }
bool Keyboard::pressed (Key k) const { return m_pressed [static_cast<size_t>(k)]; }
bool Keyboard::released(Key k) const { return m_released[static_cast<size_t>(k)]; }
KeyMod Keyboard::modifiers()   const { return m_mods; }

void Keyboard::_notify_key(Key k, int sc, bool down, KeyMod mods) {
    size_t i = static_cast<size_t>(k);
    m_mods = mods;
    if (down) { m_pressed[i]  = !m_down[i]; m_down[i] = true;  }
    else       { m_released[i] = m_down[i];  m_down[i] = false; }
    if (on_key) on_key(k, sc, down, mods);
}

void Keyboard::_notify_char(uint32_t cp) {
    if (on_char) on_char(cp);
}

void Keyboard::_end_frame() {
    m_pressed  .fill(false);
    m_released .fill(false);
}

} // namespace vgui
