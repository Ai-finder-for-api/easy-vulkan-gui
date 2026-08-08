#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Pipeline                                               ║
// ║  Graphics / compute pipeline state object.                      ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "shader.hpp"
#include "../math/color.hpp"
#include <memory>
#include <vector>

namespace vgui {

enum class PrimitiveTopology : uint8_t { TriangleList, TriangleStrip, LineList, PointList };
enum class CullMode           : uint8_t { None, Front, Back };
enum class FrontFace          : uint8_t { CCW, CW };
enum class FillMode           : uint8_t { Solid, Wireframe };
enum class BlendFactor        : uint8_t { Zero, One, SrcAlpha, OneMinusSrcAlpha, DstAlpha, OneMinusDstAlpha };
enum class CompareOp          : uint8_t { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };

struct BlendState {
    bool        enabled        = false;
    BlendFactor src_color      = BlendFactor::SrcAlpha;
    BlendFactor dst_color      = BlendFactor::OneMinusSrcAlpha;
    BlendFactor src_alpha      = BlendFactor::One;
    BlendFactor dst_alpha      = BlendFactor::Zero;
};

struct DepthState {
    bool      test_enable  = true;
    bool      write_enable = true;
    CompareOp compare_op   = CompareOp::Less;
};

struct PipelineDesc {
    std::shared_ptr<Shader>  vertex_shader;
    std::shared_ptr<Shader>  fragment_shader;
    std::shared_ptr<Shader>  compute_shader;   ///< Set for compute pipelines

    PrimitiveTopology topology   = PrimitiveTopology::TriangleList;
    CullMode          cull_mode  = CullMode::Back;
    FrontFace         front_face = FrontFace::CCW;
    FillMode          fill_mode  = FillMode::Solid;

    BlendState  blend {};
    DepthState  depth {};

    float line_width  = 1.0f;
};

class Pipeline {
public:
    static std::shared_ptr<Pipeline> create(const PipelineDesc& desc);

    bool        is_compute() const;
    void*       gpu_handle() const;   ///< VkPipeline / id<MTLRenderPipelineState>

private:
    Pipeline() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
