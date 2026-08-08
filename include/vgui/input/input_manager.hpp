#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: InputManager                                           ║
// ║  Aggregates Keyboard, Mouse, Gamepad and Touch into one API.    ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto& input = vgui::InputManager::get();                     ║
// ║    if (input.key_pressed(vgui::Key::Escape)) app.quit();        ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "keyboard.hpp"
#include "mouse.hpp"
#include "gamepad.hpp"
#include "touch.hpp"

namespace vgui {

class InputManager {
public:
    static InputManager& get();

    // ── Keyboard shortcuts ────────────────────────────────
    bool key_down    (Key k)         const { return Keyboard::get().is_down(k);   }
    bool key_pressed (Key k)         const { return Keyboard::get().pressed(k);   }
    bool key_released(Key k)         const { return Keyboard::get().released(k);  }
    KeyMod modifiers ()              const { return Keyboard::get().modifiers();  }

    // ── Mouse shortcuts ───────────────────────────────────
    Vec2 mouse_pos   ()              const { return Mouse::get().position();      }
    Vec2 mouse_delta ()              const { return Mouse::get().delta();         }
    Vec2 scroll      ()              const { return Mouse::get().scroll();        }
    bool mouse_down  (MouseButton b) const { return Mouse::get().is_down(b);     }
    bool mouse_click (MouseButton b) const { return Mouse::get().pressed(b);     }

    // ── Direct sub-system access ──────────────────────────
    Keyboard& keyboard() const { return Keyboard::get(); }
    Mouse&    mouse   () const { return Mouse::get();    }
    Gamepad&  gamepad () const { return Gamepad::get();  }
    Touch&    touch   () const { return Touch::get();    }

    /// Process queued events and update all sub-systems
    void update();
    /// Clear per-frame state (pressed/released flags)
    void end_frame();

private:
    InputManager() = default;
};

} // namespace vgui
