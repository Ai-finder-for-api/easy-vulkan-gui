// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Touch  –  implementation                               ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/input/touch.hpp>
#include <algorithm>

namespace vgui {

Touch& Touch::get() { static Touch s; return s; }

const std::vector<TouchPoint>& Touch::points()     const { return m_points; }
size_t                          Touch::count()      const { return m_points.size(); }
bool                            Touch::any_active() const { return !m_points.empty(); }

void Touch::_notify_began(const TouchPoint& p) {
    m_points.push_back(p);
    if (on_began) on_began(p);
}

void Touch::_notify_moved(const TouchPoint& p) {
    for (auto& tp : m_points)
        if (tp.id == p.id) { tp = p; break; }
    if (on_moved) on_moved(p);
}

void Touch::_notify_ended(int64_t id) {
    auto it = std::find_if(m_points.begin(), m_points.end(),
                           [id](const TouchPoint& p){ return p.id == id; });
    if (it != m_points.end()) {
        if (on_ended) on_ended(*it);
        m_points.erase(it);
    }
}

void Touch::_end_frame() {
    for (auto& p : m_points) { p.began = false; p.ended = false; p.delta = {}; }
}

} // namespace vgui
