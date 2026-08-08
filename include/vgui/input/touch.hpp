#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Touch                                                  ║
// ║  Multi-touch pointer input (mobile / trackpad).                 ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec2.hpp"
#include <vector>
#include <functional>
#include <cstdint>

namespace vgui {

struct TouchPoint {
    int64_t id       = 0;     ///< Platform touch identifier
    Vec2    position{};        ///< Screen pixels
    Vec2    delta   {};        ///< Movement since last frame
    float   pressure = 1.0f;  ///< 0..1
    bool    began    = false;
    bool    ended    = false;
};

class Touch {
public:
    static Touch& get();

    const std::vector<TouchPoint>& points()     const;
    size_t                         count()      const;
    bool                           any_active() const;

    std::function<void(const TouchPoint&)> on_began;
    std::function<void(const TouchPoint&)> on_moved;
    std::function<void(const TouchPoint&)> on_ended;

    void _notify_began(const TouchPoint& p);
    void _notify_moved(const TouchPoint& p);
    void _notify_ended(int64_t id);
    void _end_frame   ();

private:
    Touch() = default;
    std::vector<TouchPoint> m_points;
};

} // namespace vgui
