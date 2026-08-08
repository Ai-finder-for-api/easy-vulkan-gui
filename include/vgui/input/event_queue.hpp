#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: EventQueue                                             ║
// ║  Lock-free, typed event queue for cross-thread dispatch.        ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "keyboard.hpp"
#include "mouse.hpp"
#include <variant>
#include <vector>
#include <mutex>
#include <functional>

namespace vgui {

// ── Event types ───────────────────────────────────────────────────
struct EvKeyDown    { Key key; KeyMod mods; int scancode; };
struct EvKeyUp      { Key key; KeyMod mods; int scancode; };
struct EvChar       { uint32_t codepoint; };
struct EvMouseMove  { Vec2 position; };
struct EvMouseDown  { MouseButton btn; Vec2 position; };
struct EvMouseUp    { MouseButton btn; Vec2 position; };
struct EvScroll     { Vec2 delta; };
struct EvResize     { int width; int height; };
struct EvClose      {};
struct EvFocus      { bool focused; };

using Event = std::variant<
    EvKeyDown, EvKeyUp, EvChar,
    EvMouseMove, EvMouseDown, EvMouseUp, EvScroll,
    EvResize, EvClose, EvFocus
>;

class EventQueue {
public:
    static EventQueue& get();

    void push(Event e);

    /// Drain all pending events, calling the visitor for each.
    void drain(std::function<void(const Event&)> visitor);

    bool empty() const;

private:
    EventQueue() = default;
    mutable std::mutex      m_mutex;
    std::vector<Event>      m_events;
    std::vector<Event>      m_swap;   ///< Double-buffer to avoid lock during drain
};

} // namespace vgui
