#pragma once
#include <evg/evg.hpp>
#include <evg/lighting.hpp>
namespace evgx {
struct FrameState {
    math::mat4 view, proj;
    math::vec3<float> cam_pos;
    math::vec3<float> light_to;      // unit vector pointing toward the light
    math::vec3<float> light_color;
    float ambient;
    const Lighting* env = nullptr;   // when set, replaces the ambient term with image-based light
};
void raster_clear(Window* w, uint32_t argb);
void raster_draw(Window* w, const Model* m, const math::mat4& world, const FrameState& fs);
void raster_free_scratch();
}
