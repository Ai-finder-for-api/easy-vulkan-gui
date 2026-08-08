#include <vgui/graphics/camera.hpp>
#include <cmath>

namespace vgui {

Mat4 Camera::view_matrix() const {
    return Mat4::look_at(position, position + forward(), Vec3::up());
}

Mat4 Camera::proj_matrix() const {
    if (orthographic) {
        float h = ortho_size, w = h * aspect;
        return Mat4::ortho(-w, w, -h, h, near_clip, far_clip);
    }
    float fov_rad = fov_y * 3.14159265f / 180.0f;
    return Mat4::perspective(fov_rad, aspect, near_clip, far_clip);
}

Mat4 Camera::view_proj_matrix() const { return proj_matrix() * view_matrix(); }

void Camera::look_at(const Vec3& target, const Vec3& up) {
    Vec3 fwd = (target - position).normalized();
    rotation = Quat::from_axis_angle(Vec3::cross(Vec3::forward(), fwd).normalized(),
                                      std::acos(Vec3::dot(Vec3::forward(), fwd)));
}

Vec3 Camera::forward() const { return rotation * Vec3::forward(); }
Vec3 Camera::right()   const { return rotation * Vec3::right(); }
Vec3 Camera::up()      const { return rotation * Vec3::up(); }

void Camera::translate_local(const Vec3& d) {
    position += right() * d.x + up() * d.y + forward() * d.z;
}

void Camera::unproject_ray(Vec3 screen_pos, Vec3& ray_origin, Vec3& ray_dir) const {
    ray_origin = position;
    // NDC
    float nx = screen_pos.x * 2.0f - 1.0f;
    float ny = 1.0f - screen_pos.y * 2.0f;
    Mat4 inv = view_proj_matrix().inversed();
    Vec4 near4 = inv * Vec4{nx, ny, -1, 1};
    Vec4 far4  = inv * Vec4{nx, ny,  1, 1};
    Vec3 near3 = Vec3{near4.x, near4.y, near4.z} * (1.0f / near4.w);
    Vec3 far3  = Vec3{far4.x,  far4.y,  far4.z } * (1.0f / far4.w);
    ray_dir = (far3 - near3).normalized();
}

} // namespace vgui
