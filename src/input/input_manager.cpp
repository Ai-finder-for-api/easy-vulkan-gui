// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: InputManager  –  implementation                        ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/input/input_manager.hpp>
#include <vgui/input/event_queue.hpp>

namespace vgui {

InputManager& InputManager::get() { static InputManager s; return s; }

void InputManager::update() {
    Gamepad::get()._poll();

    EventQueue::get().drain([](const Event& e) {
        std::visit([](auto&& ev) {
            using T = std::decay_t<decltype(ev)>;

            if constexpr (std::is_same_v<T, EvKeyDown>)
                Keyboard::get()._notify_key(ev.key, ev.scancode, true,  ev.mods);
            else if constexpr (std::is_same_v<T, EvKeyUp>)
                Keyboard::get()._notify_key(ev.key, ev.scancode, false, ev.mods);
            else if constexpr (std::is_same_v<T, EvChar>)
                Keyboard::get()._notify_char(ev.codepoint);
            else if constexpr (std::is_same_v<T, EvMouseMove>)
                Mouse::get()._notify_move(ev.position);
            else if constexpr (std::is_same_v<T, EvMouseDown>)
                Mouse::get()._notify_button(ev.btn, true);
            else if constexpr (std::is_same_v<T, EvMouseUp>)
                Mouse::get()._notify_button(ev.btn, false);
            else if constexpr (std::is_same_v<T, EvScroll>)
                Mouse::get()._notify_scroll(ev.delta);
        }, e);
    });
}

void InputManager::end_frame() {
    Keyboard::get()._end_frame();
    Mouse::get()._end_frame();
    Touch::get()._end_frame();
}

} // namespace vgui
