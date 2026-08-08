#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "vgui/math/vec2.hpp"

#ifdef VGUI_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(VGUI_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>

namespace vgui {

/**
 * @brief Texture usage flags
 */
enum class TextureUsage : uint32_t {
    ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    DepthStencil = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    TransferSrc = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
    TransferDst = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
    Sampled = VK_IMAGE_USAGE_SAMPLED_BIT,
    Storage = VK_IMAGE_USAGE_STORAGE_BIT,
    Transient = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT
};

/**
 * @brief Texture memory properties
 */
enum class MemoryProperty : uint32_t {
    DeviceLocal = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    HostVisible = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    HostCoherent = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    HostCached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT
};

/**
 * @brief Texture creation flags
 */
enum class TextureFlag {
    None = 0,
    GenerateMipmaps = 1 << 0,
    CubeMap = 1 << 1,
    Compressed = 1 << 2,
    Volume = 1 << 3,
    UserCreated = 1 << 4  // User manages lifetime
};

/**
 * @brief Image format
 */
enum class Format : uint32_t {
    Unknown = VK_FORMAT_UNDEFINED,
    
    // 8-bit formats
    R8 = VK_FORMAT_R8_UNORM,
    R8Snorm = VK_FORMAT_R8_SNORM,
    R8U = VK_FORMAT_R8_UINT,
    R8I = VK_FORMAT_R8_SINT,
    A8 = VK_FORMAT_A8_UNORM,
    
    // 16-bit formats
    R16 = VK_FORMAT_R16_UNORM,
    R16F = VK_FORMAT_R16_SFLOAT,
    R16U = VK_FORMAT_R16_UINT,
    R16I = VK_FORMAT_R16_SINT,
    RG16 = VK_FORMAT_RG16_UNORM,
    RG16F = VK_FORMAT_RG16_SFLOAT,
    RG16U = VK_FORMAT_RG16_UINT,
    RG16I = VK_FORMAT_RG16_SINT,
    
    // 32-bit formats
    R32F = VK_FORMAT_R32_SFLOAT,
    R32U = VK_FORMAT_R32_UINT,
    R32I = VK_FORMAT_R32_SINT,
    RG32F = VK_FORMAT_RG32_SFLOAT,
    RG32U = VK_FORMAT_RG32_UINT,
    RG32I = VK_FORMAT_RG32_SINT,
    
    // Common color formats
    RGBA8 = VK_FORMAT_R8G8B8A8_UNORM,
    RGBA8Srgb = VK_FORMAT_R8G8B8A8_SRGB,
    RGBA8U = VK_FORMAT_R8G8B8A8_UINT,
    RGBA8I = VK_FORMAT_R8G8B8A8_SINT,
    BGRA8 = VK_FORMAT_B8G8R8A8_UNORM,
    BGRA8Srgb = VK_FORMAT_B8G8R8A8_SRGB,
    
    // RGB formats
    RGB8 = VK_FORMAT_R8G8B8_UNORM,
    RGB8Srgb = VK_FORMAT_R8G8B8_SRGB,
    RGB10A2 = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
    
    // Depth/stencil formats
    D16 = VK_FORMAT_D16_UNORM,
    D24S8 = VK_FORMAT_D24_UNORM_S8_UINT,
    D32F = VK_FORMAT_D32_SFLOAT,
    D32FS8 = VK_FORMAT_D32_SFLOAT_S8_UINT,
    S8 = VK_FORMAT_S8_UINT,
    
    // BPTC/BC compression (requires feature enablement)
    BC1_RGBA = VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
    BC1_RGBA_Srgb = VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
    BC2_RGBA = VK_FORMAT_BC2_UNORM_BLOCK,
    BC2_RGBA_Srgb = VK_FORMAT_BC2_SRGB_BLOCK,
    BC3_RGBA = VK_FORMAT_BC3_UNORM_BLOCK,
    BC3_RGBA_Srgb = VK_FORMAT_BC3_SRGB_BLOCK,
    
    // ETC2 (mobile)
    ETC2_RGB8 = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
    ETC2_RGB8Srgb = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
    ETC2_RGBA8 = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
    ETC2_RGBA8Srgb = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
    
    // ASTC (mobile)
    ASTC_4x4 = VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
    ASTC_4x4Srgb = VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
    ASTC_8x8 = VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
    ASTC_8x8Srgb = VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
    
    // PVRTC (mobile)
    PVRTC2_RGBA = VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
    PVRTC4_RGBA = VK_FORMAT_PVRTC4_4BPP_UNORM_BLOCK_IMG,
    
    // BPTC floating point (PBR)
    BC6H_RGB float = VK_FORMAT_BC6H_UFLOAT_BLOCK,
    BC6H_RGB_f16 = VK_FORMAT_BC6H_SFLOAT_BLOCK,
    BC7_RGBA = VK_FORMAT_BC7_UNORM_BLOCK,
};

/**
 * @brief Texture class
 * 
 * Manages image resources with support for 2D, cube, and 3D textures.
 * Handles mipmaps, compression, and efficient GPU memory management.
 */
class Texture {
public:
    /**
     * @brief Create an empty texture
     */
    Texture();
    
    /**
     * @brief Create a texture from image data
     * @param data Image data in host memory
     * @param width Width in pixels
     * @param height Height in pixels
     * @param format Format
     * @param flags Creation flags
     */
    Texture(const void* data, uint32_t width, uint32_t height,
            Format format = Format::RGBA8, uint32_t flags = TextureFlag::GenerateMipmaps);
    
    /**
     * @brief Create from Image object
     */
    Texture(std::shared_ptr<class Image> image, uint32_t flags = TextureFlag::None);
    
    /**
     * @brief Destroy the texture
     */
    ~Texture();
    
    // Prevent copying
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    
    // Allow moving
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    
    /**
     * @brief Check if texture is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get the Vulkan image handle
     */
    VkImage getImage() const { return image_; }
    
    /**
     * @brief Get the Vulkan image view
     */
    VkImageView getImageView() const { return imageView_; }
    
    /**
     * @brief Get the sampler
     */
    VkSampler getSampler() const { return sampler_; }
    
    /**
     * @brief Get width
     */
    uint32_t getWidth() const { return width_; }
    
    /**
     * @brief Get height
     */
    uint32_t getHeight() const { return height_; }
    
    /**
     * @brief Get depth (for 3D textures)
     */
    uint32_t getDepth() const { return depth_; }
    
    /**
     * @brief Get total array layers
     */
    uint32_t getLayerCount() const { return layerCount_; }
    
    /**
     * @brief Get mipmap levels
     */
    uint32_t getMipLevels() const { return mipLevels_; }
    
    /**
     * @brief Get format
     */
    Format getFormat() const { return format_; }
    
    /**
     * @brief Check if cube map
     */
    bool isCubeMap() const { return (flags_ & TextureFlag::CubeMap) != 0; }
    
    /**
     * @brief Check if has mipmaps
     */
    bool hasMipmaps() const { return mipLevels_ > 1; }
    
    /**
     * @brief Get min/mag filtering
     */
    VkFilter getMinFilter() const { return minFilter_; }
    VkFilter getMagFilter() const { return magFilter_; }
    
    /**
     * @brief Set filtering
     */
    void setFilter(VkFilter min, VkFilter mag);
    
    /**
     * @brief Set wrapping mode
     */
    void setWrapping(VkSamplerAddressMode addressMode);
    
    /**
     * @brief Load texture from file
     * @param path File path
     * @return true on success
     */
    bool loadFromFile(const std::string& path,
                      uint32_t flags = TextureFlag::GenerateMipmaps);
    
    /**
     * @brief Load texture from memory (stb_image compatible)
     * @param data Pointer to image data
     * @param size Size of data in bytes
     * @param flags Creation flags
     * @return true on success
     */
    bool loadFromMemory(const void* data, size_t size,
                        uint32_t flags = TextureFlag::GenerateMipmaps);
    
    /**
     * @brief Generate mipmaps
     */
    bool generateMipmaps();
    
    /**
     * @brief Get the image memory
     */
    VkDeviceMemory getMemory() const { return memory_; }
    
    /**
     * @brief Map texture memory for CPU access
     */
    void* map(uint32_t mipLevel = 0, uint32_t layer = 0);
    
    /**
     * @brief Unmap texture memory
     */
    void unmap();
    
    /**
     * @brief Flush mapped memory ranges
     */
    void flush();
    
private:
    bool valid_ = false;
    VkImage image_ = VK_NULL_HANDLE;
    VkImageView imageView_ = VK_NULL_HANDLE;
    VkSampler sampler_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t depth_ = 1;
    uint32_t layerCount_ = 1;
    uint32_t mipLevels_ = 1;
    Format format_ = Format::RGBA8;
    uint32_t flags_ = TextureFlag::None;
    
    VkFilter minFilter_ = VK_FILTER_LINEAR;
    VkFilter magFilter_ = VK_FILTER_LINEAR;
    VkSamplerAddressMode addressMode_ = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    // Memory mapping
    void* mappedData_ = nullptr;
    size_t mappedSize_ = 0;
    bool mapped_ = false;
};

/**
 * @brief Texture array for efficient batch rendering
 */
class TextureArray {
public:
    /**
     * @brief Create an empty texture array
     */
    TextureArray();
    
    /**
     * @brief Add a texture to the array
     * @param texture Texture to add (must have same dimensions and format)
     */
    bool addTexture(const Texture& texture);
    
    /**
     * @brief Build the array from added textures
     */
    bool build();
    
    /**
     * @brief Get the underlying texture
     */
    const Texture& getTexture() const { return baseTexture_; }
    
    /**
     * @brief Get layer count
     */
    uint32_t getLayerCount() const { return baseTexture_.getLayerCount(); }
    
private:
    std::vector<Texture> textures_;
    Texture baseTexture_;
    bool built_ = false;
};

} // namespace vgui
