#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#ifdef VGUI_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(VGUI_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>

#include "vgui/graphics/texture.hpp"
#include "vgui/graphics/renderpass.hpp"

namespace vgui {

/**
 * @brief Framebuffer configuration
 */
struct FramebufferConfig {
    // Render pass
    std::shared_ptr<RenderPass> renderPass;
    uint32_t subpassIndex = 0;
    
    // Attachments
    struct Attachment {
        std::shared_ptr<Texture> texture;
        uint32_t attachmentIndex = 0;  // Index in render pass
        uint32_t layerCount = 1;
        uint32_t mipLevel = 0;
    };
    
    std::vector<Attachment> colorAttachments;
    std::shared_ptr<Texture> depthAttachment;
    std::shared_ptr<Texture> stencilAttachment;
    
    // Dimensions
    uint32_t width = 0;
    uint32_t height = 0;
    
    // Debug name
    std::string debugName;
    
    /**
     * @brief Create framebuffer for render to texture
     */
    static FramebufferConfig createForColorAttachment(
        std::shared_ptr<Texture> colorTexture,
        uint32_t width, uint32_t height,
        std::shared_ptr<RenderPass> renderPass);
    
    /**
     * @brief Create framebuffer for offscreen rendering
     */
    static FramebufferConfig createForOffscreen(
        std::shared_ptr<Texture> colorTexture,
        std::shared_ptr<Texture> depthTexture,
        uint32_t width, uint32_t height,
        std::shared_ptr<RenderPass> renderPass);
};

/**
 * @brief Framebuffer class
 * 
 * Represents a complete framebuffer for rendering.
 * Can be used for offscreen rendering or as main swapchain framebuffer.
 */
class Framebuffer {
public:
    /**
     * @brief Create an empty framebuffer
     */
    Framebuffer();
    
    /**
     * @brief Create from config
     */
    bool create(const FramebufferConfig& config);
    
    /**
     * @brief Create from render pass and attachments
     */
    bool create(std::shared_ptr<RenderPass> renderPass,
                const std::vector<VkImageView>& attachments,
                uint32_t width, uint32_t height);
    
    /**
     * @brief Destroy
     */
    ~Framebuffer();
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkFramebuffer getHandle() const { return framebuffer_; }
    
    /**
     * @brief Get width
     */
    uint32_t getWidth() const { return width_; }
    
    /**
     * @brief Get height
     */
    uint32_t getHeight() const { return height_; }
    
    /**
     * @brief Get render pass
     */
    std::shared_ptr<RenderPass> getRenderPass() const { return renderPass_; }
    
    /**
     * @brief Recreate with new dimensions
     */
    bool recreate(uint32_t width, uint32_t height);
    
    /**
     * @brief Get color attachment textures
     */
    const std::vector<std::shared_ptr<Texture>>& getColorAttachments() const {
        return colorAttachments_;
    }
    
    /**
     * @brief Get depth attachment
     */
    std::shared_ptr<Texture> getDepthAttachment() const { return depthAttachment_; }
    
private:
    void destroy();
    
    bool valid_ = false;
    VkFramebuffer framebuffer_ = VK_NULL_HANDLE;
    std::shared_ptr<RenderPass> renderPass_;
    
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    
    std::vector<std::shared_ptr<Texture>> colorAttachments_;
    std::shared_ptr<Texture> depthAttachment_;
    std::string debugName_;
};

/**
 * @brief Framebuffer manager
 * 
 * Manages multiple framebuffers for render-to-texture operations.
 */
class FramebufferManager {
public:
    /**
     * @brief Get instance
     */
    static FramebufferManager& getInstance();
    
    /**
     * @brief Create framebuffer
     */
    std::shared_ptr<Framebuffer> create(const std::string& name,
                                        const FramebufferConfig& config);
    
    /**
     * @brief Get framebuffer by name
     */
    std::shared_ptr<Framebuffer> get(const std::string& name);
    
    /**
     * @brief Check if exists
     */
    bool has(const std::string& name) const;
    
    /**
     * @brief Remove framebuffer
     */
    void remove(const std::string& name);
    
    /**
     * @brief Clear all
     */
    void clear();
    
    /**
     * @brief Get framebuffer count
     */
    size_t getCount() const { return framebuffers_.size(); }
    
private:
    FramebufferManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Framebuffer>> framebuffers_;
};

} // namespace vgui
