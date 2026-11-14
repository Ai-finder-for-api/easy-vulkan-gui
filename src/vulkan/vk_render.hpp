// GPU rendering of meshes through a graphics pipeline. Internal to src/vulkan; the public entry points are in
// vulkan_draw_api.cpp (Api::vulkan_draw, Api::vulkan_render_pixels).
#pragma once
#include <stdint.h>

namespace evgx {
struct Model;

namespace vkp {

// draws `m` into the window's swapchain (rendered offscreen, then copied and presented)
bool render_to_window(void* hinst, void* hwnd, uint32_t w, uint32_t h, const Model* m, const float viewProj[16],
                      float r, float g, float b, const char** err);

// renders `m` into an offscreen target and reads the pixels back as 0xAARRGGBB, row-major, top row first
bool render_to_pixels(uint32_t w, uint32_t h, const Model* m, const float viewProj[16], float r, float g, float b,
                      uint32_t* out, const char** err);

// frees the GPU copy of a model (called from the model free path; safe for models never drawn)
void free_model_gpu(const Model* m);

// destroys all render-path objects (pipeline, render pass, targets, model buffers); called before the device goes
void render_shutdown();

}}  // namespace evgx::vkp
