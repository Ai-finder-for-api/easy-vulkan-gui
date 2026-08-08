#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Sampler                                                ║
// ║  GPU texture sampler (filter, wrap, anisotropy).               ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <memory>

namespace vgui {

enum class FilterMode : uint8_t {
    Nearest,     ///< Pixelated / no filtering
    Linear,      ///< Bilinear
    Trilinear    ///< Trilinear (linear + mip-linear)
};

enum class WrapMode : uint8_t {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
};

struct SamplerDesc {
    FilterMode  min_filter  = FilterMode::Linear;
    FilterMode  mag_filter  = FilterMode::Linear;
    FilterMode  mip_filter  = FilterMode::Linear;
    WrapMode    wrap_u      = WrapMode::Repeat;
    WrapMode    wrap_v      = WrapMode::Repeat;
    WrapMode    wrap_w      = WrapMode::Repeat;
    float       anisotropy  = 1.0f;            ///< 1..16; 1 = disabled
    float       mip_bias    = 0.0f;
    float       min_lod     = 0.0f;
    float       max_lod     = 1000.0f;
};

class Sampler {
public:
    static std::shared_ptr<Sampler> create(const SamplerDesc& desc = {});

    /// Pre-built common samplers
    static std::shared_ptr<Sampler> nearest();
    static std::shared_ptr<Sampler> linear();
    static std::shared_ptr<Sampler> anisotropic(float level = 16.0f);

    void* gpu_handle() const;  ///< VkSampler / id<MTLSamplerState>

private:
    Sampler() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
