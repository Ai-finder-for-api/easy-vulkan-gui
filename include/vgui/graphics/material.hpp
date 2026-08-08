#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Material                                               ║
// ║  PBR material: textures + scalar parameters.                    ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "texture.hpp"
#include "sampler.hpp"
#include "pipeline.hpp"
#include "../math/color.hpp"
#include <memory>
#include <string>

namespace vgui {

class Material {
public:
    static std::shared_ptr<Material> create();

    /// Load a material from a simple JSON descriptor file
    static std::shared_ptr<Material> load(const std::string& path);

    // ── PBR parameters ────────────────────────────────────
    Color  albedo           = Color::white();
    float  metallic         = 0.0f;
    float  roughness        = 0.5f;
    float  ambient_occlusion= 1.0f;
    Color  emissive         = Color::black();
    float  emissive_strength= 1.0f;
    float  alpha_cutoff     = 0.5f;
    bool   double_sided     = false;

    // ── Texture slots ─────────────────────────────────────
    std::shared_ptr<Texture> albedo_map;
    std::shared_ptr<Texture> normal_map;
    std::shared_ptr<Texture> metallic_roughness_map;
    std::shared_ptr<Texture> ao_map;
    std::shared_ptr<Texture> emissive_map;

    // ── Pipeline override (optional) ──────────────────────
    std::shared_ptr<Pipeline> pipeline;

    void bind(class CommandBuffer& cmd, uint32_t set_slot = 1);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
