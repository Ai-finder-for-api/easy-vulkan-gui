#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: CommandBuffer                                          ║
// ║  Records GPU commands for a single frame.                       ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "pipeline.hpp"
#include "descriptor.hpp"
#include "renderpass.hpp"
#include "framebuffer.hpp"
#include "buffer.hpp"
#include "../math/vec2.hpp"
#include "../math/color.hpp"
#include <memory>

namespace vgui {

class CommandBuffer {
public:
    static std::shared_ptr<CommandBuffer> create();

    void begin();
    void end();
    void submit();

    // ── Render pass ───────────────────────────────────────
    void begin_pass(RenderPass& pass, Framebuffer& fb);
    void begin_pass(RenderPass& pass);      ///< Renders to swapchain
    void end_pass();

    // ── Pipeline & resources ──────────────────────────────
    void bind_pipeline  (Pipeline&      pipeline);
    void bind_descriptors(DescriptorSet& ds);
    void bind_vertex    (Buffer& vbo, uint32_t binding = 0);
    void bind_index     (Buffer& ibo, bool use_32bit = false);

    // ── Draw calls ────────────────────────────────────────
    void draw           (uint32_t vertex_count, uint32_t first = 0);
    void draw_indexed   (uint32_t index_count,  uint32_t first = 0, int32_t vertex_offset = 0);
    void draw_instanced (uint32_t vertex_count, uint32_t instances);
    void dispatch       (uint32_t gx, uint32_t gy, uint32_t gz = 1);  ///< Compute

    // ── Dynamic state ─────────────────────────────────────
    void set_viewport(float x, float y, float w, float h, float min_depth = 0, float max_depth = 1);
    void set_scissor (int x, int y, int w, int h);
    void set_line_width(float w);

    // ── Push constants (Vulkan) / inline buffer (Metal) ───
    void push_constants(const void* data, uint32_t size, uint32_t offset = 0);

    void* gpu_handle() const;

private:
    CommandBuffer() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
