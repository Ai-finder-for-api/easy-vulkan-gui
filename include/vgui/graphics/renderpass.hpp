#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: RenderPass                                             ║
// ║  Describes render targets and load/store ops.                   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "texture.hpp"
#include "../math/color.hpp"
#include <memory>
#include <vector>

namespace vgui {

enum class LoadOp  : uint8_t { Load, Clear, DontCare };
enum class StoreOp : uint8_t { Store, DontCare };

struct Attachment {
    std::shared_ptr<Texture> texture;
    LoadOp   load_op   = LoadOp::Clear;
    StoreOp  store_op  = StoreOp::Store;
    Color    clear_color = Color::black();   ///< Used when load_op == Clear
    float    clear_depth   = 1.0f;
    uint8_t  clear_stencil = 0;
};

struct RenderPassDesc {
    std::vector<Attachment> color_attachments;
    Attachment              depth_attachment;
    bool                    use_depth = true;
};

class RenderPass {
public:
    static std::shared_ptr<RenderPass> create(const RenderPassDesc& desc);

    void* gpu_handle() const;   ///< VkRenderPass / MTLRenderPassDescriptor

private:
    RenderPass() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
