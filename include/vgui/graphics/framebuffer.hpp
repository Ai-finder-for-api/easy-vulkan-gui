#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Framebuffer                                            ║
// ║  Off-screen render target backed by textures.                   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "texture.hpp"
#include "renderpass.hpp"
#include <memory>
#include <vector>

namespace vgui {

class Framebuffer {
public:
    static std::shared_ptr<Framebuffer> create(
        int                               width,
        int                               height,
        const std::vector<TextureFormat>& color_formats,
        TextureFormat                     depth_format = TextureFormat::Depth24Stencil8);

    int width()  const;
    int height() const;

    Texture& color_texture(int index = 0);
    Texture& depth_texture();

    void resize(int w, int h);

    void* gpu_handle() const;    ///< VkFramebuffer / (Metal: uses textures directly)

private:
    Framebuffer() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
