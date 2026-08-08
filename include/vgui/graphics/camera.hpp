#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Camera                                                 ║
// ║  Perspective and orthographic camera with view/proj matrices.   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec3.hpp"
#include "../math/mat4.hpp"
#include "../math/quat.hpp"

namespace vgui {

class Camera {
public:
    // ── Transform ─────────────────────────────────────────
    Vec3  position  = Vec3::zero();
    Quat  rotation  = Quat::identity();

    // ── Projection ────────────────────────────────────────
    float fov_y     = 60.0f;   ///< Vertical field of view in degrees
    float near_clip = 0.01f;
    float far_clip  = 1000.0f;
    float aspect    = 16.0f / 9.0f;
    bool  orthographic = false;

    // ── Ortho params (when orthographic == true) ──────────
    float ortho_size = 5.0f;   ///< Half-height in world units

    // ── Derived matrices ──────────────────────────────────
    Mat4 view_matrix()       const;
    Mat4 proj_matrix()       const;
    Mat4 view_proj_matrix()  const;

    // ── Helpers ───────────────────────────────────────────
    void look_at(const Vec3& target, const Vec3& up = Vec3::up());
    Vec3 forward() const;
    Vec3 right()   const;
    Vec3 up()      const;

    /// Move relative to local axes
    void translate_local(const Vec3& delta);

    /// Unproject a screen-space point to a world-space ray
    /// screen_pos in [0,1] range (top-left origin)
    void unproject_ray(Vec3 screen_pos, Vec3& ray_origin, Vec3& ray_dir) const;
};

} // namespace vgui
