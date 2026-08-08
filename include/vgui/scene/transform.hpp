#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Transform                                              ║
// ║  TRS (Translate/Rotate/Scale) with lazy matrix rebuild.         ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../math/vec3.hpp"
#include "../math/quat.hpp"
#include "../math/mat4.hpp"

namespace vgui {

class Transform {
public:
    Vec3  position = Vec3::zero();
    Quat  rotation = Quat::identity();
    Vec3  scale    = Vec3::one();

    // ── Matrix ────────────────────────────────────────────
    const Mat4& local_matrix()  const;
    const Mat4& world_matrix()  const;   ///< Includes parent chain

    // ── Helpers ───────────────────────────────────────────
    Vec3 forward() const { return rotation * Vec3::forward(); }
    Vec3 right()   const { return rotation * Vec3::right();   }
    Vec3 up()      const { return rotation * Vec3::up();      }

    void translate(const Vec3& delta);
    void rotate   (const Quat& q);
    void look_at  (const Vec3& target, const Vec3& world_up = Vec3::up());

    void set_parent(const Transform* parent);
    const Transform* parent() const { return m_parent; }

    void mark_dirty();

private:
    const Transform* m_parent = nullptr;
    mutable Mat4     m_local {};
    mutable Mat4     m_world {};
    mutable bool     m_dirty = true;
};

} // namespace vgui
