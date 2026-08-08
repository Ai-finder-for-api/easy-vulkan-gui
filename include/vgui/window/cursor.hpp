#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Cursor                                                 ║
// ║  System cursor shapes and custom cursor images.                 ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <memory>

namespace vgui {

enum class CursorShape : uint8_t {
    Arrow,
    IBeam,
    Crosshair,
    Hand,
    ResizeH,     ///< ← →
    ResizeV,     ///< ↑ ↓
    ResizeNWSE,  ///< ↖ ↘
    ResizeNESW,  ///< ↗ ↙
    ResizeAll,   ///< ✛
    NotAllowed,
    Wait,
    Hidden
};

class Cursor {
public:
    static Cursor& get();

    void set_shape(CursorShape shape);

    /// Create a cursor from raw RGBA pixels (hotspot in pixels from top-left)
    void set_custom(const uint8_t* rgba, int width, int height,
                    int hotspot_x = 0, int hotspot_y = 0);

    CursorShape current_shape() const;

    void reset();    ///< Back to Arrow

private:
    Cursor() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
