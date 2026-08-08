#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "vgui/math/vec2.hpp"

namespace vgui {

/**
 * @brief Image class for CPU-side image data
 * 
 * Provides loading, saving, and manipulation of image data.
 * Supports common formats via stb_image integration.
 */
class Image {
public:
    /**
     * @brief Create an empty image
     */
    Image();
    
    /**
     * @brief Create image with specified dimensions
     */
    Image(uint32_t width, uint32_t height, Format format = Format::RGBA8);
    
    /**
     * @brief Create image from raw data
     */
    Image(const void* data, uint32_t width, uint32_t height, 
          Format format = Format::RGBA8, uint32_t channels = 4);
    
    /**
     * @brief Destroy the image
     */
    ~Image();
    
    /**
     * @brief Load image from file
     * @param path File path
     * @param desiredChannels Desired number of channels (0 = auto)
     * @return true on success
     */
    bool loadFromFile(const std::string& path, int desiredChannels = 0);
    
    /**
     * @brief Load image from memory
     */
    bool loadFromMemory(const void* data, size_t size, int desiredChannels = 0);
    
    /**
     * @brief Save image to file
     */
    bool saveToFile(const std::string& path, uint32_t quality = 90) const;
    
    /**
     * @brief Create image from raw data
     */
    void setData(const void* data, uint32_t width, uint32_t height,
                 Format format = Format::RGBA8, uint32_t channels = 4);
    
    /**
     * @brief Get pixel data
     */
    const void* getData() const { return data_; }
    void* getData() { return data_; }
    
    /**
     * @brief Get image dimensions
     */
    uint32_t getWidth() const { return width_; }
    uint32_t getHeight() const { return height_; }
    Vec2u getSize() const { return Vec2u(width_, height_); }
    
    /**
     * @brief Get format
     */
    Format getFormat() const { return format_; }
    
    /**
     * @brief Get number of channels
     */
    uint32_t getChannels() const { return channels_; }
    
    /**
     * @brief Get row stride in bytes
     */
    uint32_t getStride() const { return stride_; }
    
    /**
     * @brief Get pitch (row size in bytes, may include padding)
     */
    uint32_t getPitch() const { return width_ * channels_; }
    
    /**
     * @brief Check if image is valid
     */
    bool isValid() const { return data_ != nullptr && width_ > 0 && height_ > 0; }
    
    /**
     * @brief Resize image
     */
    void resize(uint32_t newWidth, uint32_t newHeight,
                uint32_t filter = 0); // 0 = bilinear
    
    /**
     * @brief Flip image vertically
     */
    void flipVertical();
    
    /**
     * @brief Flip image horizontally
     */
    void flipHorizontal();
    
    /**
     * @brief Convert to different format
     */
    void convertTo(Format newFormat);
    
    /**
     * @brief Get pixel color at (u,v) where u,v are in [0,1]
     */
    vgui::Colorf getPixelUV(float u, float v) const;
    
    /**
     * @brief Set pixel color at (u,v)
     */
    void setPixelUV(float u, float v, const vgui::Colorf& color);
    
private:
    void* data_ = nullptr;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t channels_ = 4;
    uint32_t stride_ = 0;
    Format format_ = Format::RGBA8;
    bool ownsData_ = true;
};

/**
 * @brief Image views for sub-image access
 */
class ImageView {
public:
    ImageView(const Image& image, uint32_t x, uint32_t y, 
              uint32_t width, uint32_t height);
    
    const void* getData() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getChannels() const;
    
private:
    const Image* image_;
    uint32_t x_;
    uint32_t y_;
    uint32_t width_;
    uint32_t height_;
};

} // namespace vgui
