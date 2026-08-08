#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Vertex                                                 ║
// ║  Standard vertex layouts.                                       ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec2.hpp"
#include "../math/vec3.hpp"
#include "../math/vec4.hpp"
#include "../math/color.hpp"
#include <cstdint>

namespace vgui {

/// 3-D mesh vertex (pos + normal + uv + tangent)
struct Vertex3D {
    Vec3  position  {};
    Vec3  normal    {};
    Vec2  uv        {};
    Vec4  tangent   {};   ///< w = handedness (+1 or -1)

    bool operator==(const Vertex3D& o) const = default;
};

/// 2-D UI vertex (pos + uv + colour)
struct Vertex2D {
    Vec2  position  {};
    Vec2  uv        {};
    Color color     = Color::white();

    bool operator==(const Vertex2D& o) const = default;
};

/// Skinned mesh vertex (adds bone indices + weights)
struct VertexSkinned : Vertex3D {
    uint8_t bone_ids    [4] {};
    float   bone_weights[4] {};
};

} // namespace vgui
