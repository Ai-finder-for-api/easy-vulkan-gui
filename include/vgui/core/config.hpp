#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: AppConfig                                              ║
// ║  All startup settings in one plain struct.                      ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    vgui::AppConfig cfg;                                         ║
// ║    cfg.width  = 1920;                                           ║
// ║    cfg.height = 1080;                                           ║
// ║    cfg.vsync  = true;                                           ║
// ║    vgui::Application::init(cfg);                                ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string>
#include <cstdint>

namespace vgui {

struct AppConfig {
    // ── Window ────────────────────────────────────────────
    std::string  title      = "vgui App";
    int          width      = 1280;
    int          height     = 720;
    bool         resizable  = true;
    bool         vsync      = true;
    bool         fullscreen = false;
    int          msaa       = 1;        ///< 1, 2, 4, or 8 samples

    // ── Renderer ──────────────────────────────────────────
    bool         validation = false;    ///< Enable GPU validation layers
    uint32_t     max_frames = 2;        ///< Frames in flight

    // ── UI ────────────────────────────────────────────────
    float        ui_scale   = 1.0f;
    std::string  font_path  = "";       ///< Empty = built-in bitmap font

    // ── Logging ───────────────────────────────────────────
    bool         log_file   = false;
    std::string  log_path   = "vgui.log";
};

} // namespace vgui
