#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Light                                                  ║
// ║  Directional, point, and spot lights.                           ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec3.hpp"
#include "../math/color.hpp"

namespace vgui {

enum class LightType : uint8_t { Directional, Point, Spot };

struct Light {
    LightType type       = LightType::Point;
    Color     color      = Color::white();
    float     intensity  = 1.0f;

    // ── Point / Spot ──────────────────────────────────────
    float     range      = 10.0f;

    // ── Spot ──────────────────────────────────────────────
    float     inner_cone = 15.0f;   ///< Degrees
    float     outer_cone = 30.0f;   ///< Degrees

    // ── Shadows ───────────────────────────────────────────
    bool      cast_shadow     = false;
    int       shadow_map_size = 1024;
    float     shadow_bias     = 0.002f;

    // ── Baked ─────────────────────────────────────────────
    bool      baked = false;
};

} // namespace vgui
