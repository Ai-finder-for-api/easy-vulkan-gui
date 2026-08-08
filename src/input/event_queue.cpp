// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: EventQueue  –  implementation                          ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/input/event_queue.hpp>

namespace vgui {

EventQueue& EventQueue::get() { static EventQueue s; return s; }

void EventQueue::push(Event e) {
    std::lock_guard lock(m_mutex);
    m_events.push_back(std::move(e));
}

void EventQueue::drain(std::function<void(const Event&)> visitor) {
    {   std::lock_guard lock(m_mutex);
        std::swap(m_events, m_swap); }
    for (auto& e : m_swap) visitor(e);
    m_swap.clear();
}

bool EventQueue::empty() const {
    std::lock_guard lock(m_mutex);
    return m_events.empty();
}

} // namespace vgui
