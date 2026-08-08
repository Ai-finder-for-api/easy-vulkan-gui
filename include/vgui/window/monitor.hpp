#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Monitor                                                ║
// ║  Query connected display information.                           ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec2.hpp"
#include <string>
#include <vector>

namespace vgui {

struct VideoMode {
    int   width      = 0;
    int   height     = 0;
    int   refresh_hz = 60;
    int   red_bits   = 8;
    int   green_bits = 8;
    int   blue_bits  = 8;
};

struct Monitor {
    std::string             name;
    Vec2                    position;        ///< Virtual-desktop origin
    Vec2                    physical_mm;     ///< Physical size in millimetres
    float                   content_scale = 1.0f;
    VideoMode               current_mode;
    std::vector<VideoMode>  video_modes;
    bool                    is_primary = false;
    void*                   native_handle = nullptr;

    // ── Helpers ───────────────────────────────────────────
    float dpi_x() const { return physical_mm.x > 0 ? current_mode.width  / (physical_mm.x/25.4f) : 96.0f; }
    float dpi_y() const { return physical_mm.y > 0 ? current_mode.height / (physical_mm.y/25.4f) : 96.0f; }
};

class MonitorManager {
public:
    static MonitorManager& get();

    void refresh();                              ///< Re-query from OS
    const std::vector<Monitor>& all()   const;
    const Monitor&              primary() const;

private:
    MonitorManager() = default;
    std::vector<Monitor> m_monitors;
};

} // namespace vgui
