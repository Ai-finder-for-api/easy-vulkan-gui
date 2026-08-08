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

namespace vgui {

/**
 * @brief Attachment description
 */
struct AttachmentDescription {
    uint32_t format = VK_FORMAT_B8G8R8A8_UNORM;
    VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    uint32_t samples = 1;
    
    // For depth/stencil attachments
    bool isDepth = false;
    bool isStencil = false;
    
    // Clear value
    VkClearValue clearValue;
    
    AttachmentDescription() {
        clearValue.color.float32[0] = 0.0f;
        clearValue.color.float32[1] = 0.0f;
        clearValue.color.float32[2] = 0.0f;
        clearValue.color.float32[3] = 1.0f;
    }
};

/**
 * @brief Subpass description
 */
struct SubpassDescription {
    std::vector<uint32_t> colorAttachments;
    uint32_t depthStencilAttachment = VK_ATTACHMENT_REFERENCE_UNUSED;
    std::vector<uint32_t> inputAttachments;
    std::vector<uint32_t> preserveAttachments;
    std::vector<uint32_t> resolveAttachments;
    
    VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    
    // References to other subpasses (for subpass dependencies)
    std::vector<uint32_t> references;
};

/**
 * @brief Subpass dependency
 */
struct SubpassDependency {
    uint32_t srcSubpass = 0;
    uint32_t dstSubpass = 0;
    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkAccessFlagBits srcAccessMask = 0;
    VkAccessFlagBits dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkDependencyFlags dependencyFlags = 0;
};

/**
 * @brief Render pass configuration
 */
struct RenderPassConfig {
    std::string debugName;
    
    // Attachments
    std::vector<AttachmentDescription> attachments;
    
    // Subpasses
    std::vector<SubpassDescription> subpasses;
    
    // Dependencies
    std::vector<SubpassDependency> dependencies;
    
    // External dependencies for swapchain
    bool addExternalDependency = true;
    
    /**
     * @brief Create a simple render pass with one color attachment
     */
    static RenderPassConfig createSimpleColor(uint32_t format,
                                               bool clear = true);
    
    /**
     * @brief Create render pass with depth
     */
    static RenderPassConfig createWithDepth(uint32_t colorFormat,
                                            uint32_t depthFormat = VK_FORMAT_D32_SFLOAT,
                                            bool clear = true);
    
    /**
     * @brief Create MSAA render pass
     */
    static RenderPassConfig createMSAA(uint32_t colorFormat,
                                       uint32_t resolveFormat,
                                       uint32_t depthFormat = VK_FORMAT_D32_SFLOAT,
                                       uint32_t samples = 4);
    
    /**
     * @brief Create UI render pass (no depth, single color)
     */
    static RenderPassConfig createUI(uint32_t format);
};

/**
 * @brief Render pass class
 */
class RenderPass {
public:
    /**
     * @brief Create empty render pass
     */
    RenderPass();
    
    /**
     * @brief Create from config
     */
    bool create(const RenderPassConfig& config);
    
    /**
     * @brief Destroy
     */
    ~RenderPass();
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkRenderPass getHandle() const { return renderPass_; }
    
    /**
     * @brief Get attachment count
     */
    uint32_t getAttachmentCount() const { return attachments_.size(); }
    
    /**
     * @brief Get attachment format
     */
    uint32_t getAttachmentFormat(uint32_t index) const;
    
    /**
     * @brief Get subpass count
     */
    uint32_t getSubpassCount() const { return subpasses_.size(); }
    
    /**
     * @brief Start render pass on command buffer
     */
    void begin(class CommandBuffer& cmdBuffer,
               VkFramebuffer framebuffer,
               const VkClearValue* clearValues = nullptr,
               uint32_t clearValueCount = 0);
    
    /**
     * @brief End render pass
     */
    void end(class CommandBuffer& cmdBuffer);
    
    /**
     * @brief Get clear values
     */
    const std::vector<VkClearValue>& getClearValues() const { return clearValues_; }
    
    /**
     * @brief Set clear value for attachment
     */
    void setClearValue(uint32_t attachmentIndex, const VkClearValue& value);
    
    /**
     * @brief Set clear color
     */
    void setClearColor(uint32_t attachmentIndex, float r, float g, float b, float a = 1.0f);
    
private:
    void destroy();
    void buildFromConfig(const RenderPassConfig& config);
    
    bool valid_ = false;
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    
    RenderPassConfig config_;
    std::vector<AttachmentDescription> attachments_;
    std::vector<SubpassDescription> subpasses_;
    std::vector<SubpassDependency> dependencies_;
    
    std::vector<VkClearValue> clearValues_;
};

/**
 * @brief Render pass manager
 */
class RenderPassManager {
public:
    /**
     * @brief Get instance
     */
    static RenderPassManager& getInstance();
    
    /**
     * @brief Create render pass
     */
    std::shared_ptr<RenderPass> create(const std::string& name,
                                       const RenderPassConfig& config);
    
    /**
     * @brief Get render pass
     */
    std::shared_ptr<RenderPass> get(const std::string& name);
    
    /**
     * @brief Check if exists
     */
    bool has(const std::string& name) const;
    
    /**
     * @brief Remove
     */
    void remove(const std::string& name);
    
    /**
     * @brief Get swapchain render pass
     */
    std::shared_ptr<RenderPass> getSwapchainRenderPass();
    
    /**
     * @brief Get UI render pass
     */
    std::shared_ptr<RenderPass> getUIRenderPass();
    
private:
    RenderPassManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<RenderPass>> renderPasses_;
    std::shared_ptr<RenderPass> swapchainRenderPass_;
    std::shared_ptr<RenderPass> uiRenderPass_;
};

} // namespace vgui
