#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Descriptor                                             ║
// ║  Binds buffers and textures to shader slots.                    ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    auto ds = vgui::DescriptorSet::create();                     ║
// ║    ds->bind_uniform(0, my_ubo.buffer());                        ║
// ║    ds->bind_texture(1, my_tex, my_sampler);                     ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "buffer.hpp"
#include "texture.hpp"
#include "sampler.hpp"
#include <memory>
#include <vector>

namespace vgui {

class DescriptorSet {
public:
    static std::shared_ptr<DescriptorSet> create();

    void bind_uniform(uint32_t slot, Buffer&  buffer);
    void bind_storage(uint32_t slot, Buffer&  buffer);
    void bind_texture(uint32_t slot, Texture& tex, Sampler& sampler);

    void update();    ///< Push all pending bindings to the GPU

    void* gpu_handle() const;   ///< VkDescriptorSet / (Metal: no-op, direct binding)

private:
    DescriptorSet() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
