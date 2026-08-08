#include <vgui/scene/transform.hpp>

namespace vgui {

const Mat4& Transform::local_matrix() const {
    if (m_dirty) {
        Mat4 t = Mat4::translate(position);
        Mat4 r = rotation.to_mat4();
        Mat4 s = Mat4::scale(scale);
        m_local = t * r * s;
        m_dirty = false;
    }
    return m_local;
}

const Mat4& Transform::world_matrix() const {
    if (m_parent) { m_world = m_parent->world_matrix() * local_matrix(); return m_world; }
    return local_matrix();
}

void Transform::translate(const Vec3& d) { position += d; mark_dirty(); }
void Transform::rotate   (const Quat& q) { rotation = (q * rotation).normalized(); mark_dirty(); }
void Transform::mark_dirty()             { m_dirty = true; }

void Transform::look_at(const Vec3& target, const Vec3& world_up) {
    Vec3 fwd = (target - position).normalized();
    Vec3 right= Vec3::cross(fwd, world_up).normalized();
    Vec3 up   = Vec3::cross(right, fwd);
    // Build rotation from axes
    Mat4 m = Mat4::identity();
    m.m[0][0]=right.x; m.m[0][1]=up.x; m.m[0][2]=-fwd.x;
    m.m[1][0]=right.y; m.m[1][1]=up.y; m.m[1][2]=-fwd.y;
    m.m[2][0]=right.z; m.m[2][1]=up.z; m.m[2][2]=-fwd.z;
    rotation = Quat::from_mat4(m); mark_dirty();
}

void Transform::set_parent(const Transform* p) { m_parent = p; mark_dirty(); }

} // namespace vgui
