#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Texture                                                ║
// ║  2-D / Cube / 2-D-Array GPU texture.                           ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <memory>
#include <string>

namespace vgui {

enum class TextureFormat : uint8_t {
    RGBA8,          ///< 8-bit per channel, sRGB
    RGBA8_Linear,
    RGBA16F,        ///< Half-float HDR
    RGBA32F,        ///< Full-float HDR
    RG16F,
    R8,
    R32F,
    Depth24Stencil8,
    Depth32F,
    BC1,            ///< DXT1 compressed
    BC3,            ///< DXT5 compressed
    BC7             ///< High-quality compressed
};

enum class TextureType : uint8_t { Tex2D, TexCube, Tex2DArray };

struct TextureDesc {
    int          width   = 1;
    int          height  = 1;
    int          depth   = 1;   ///< Array layers / cube faces (1 for regular)
    int          mips    = 1;   ///< 0 = auto-generate full chain
    TextureFormat format = TextureFormat::RGBA8;
    TextureType   type   = TextureType::Tex2D;
    bool          render_target = false;
    bool          storage       = false;  ///< Compute shader write
};

class Texture {
public:
    static std::shared_ptr<Texture> create(const TextureDesc& desc,
                                            const void*        data = nullptr);

    /// Load from file (PNG / JPG / BMP / TGA / HDR, built-in decoder)
    static std::shared_ptr<Texture> load(const std::string& path,
                                          bool gen_mips = true);

    ~Texture();

    int           width()   const;
    int           height()  const;
    int           mips()    const;
    TextureFormat format()  const;
    void*         gpu_handle() const;   ///< VkImageView / id<MTLTexture>

    void upload(int mip, int layer, const void* data, size_t bytes);

private:
    Texture() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
