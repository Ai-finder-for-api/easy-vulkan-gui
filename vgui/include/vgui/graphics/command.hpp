#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#ifdef VGUI_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(VGUI_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>

#include "vgui/graphics/buffer.hpp"
#include "vgui/graphics/pipeline.hpp"
#include "vgui/graphics/descriptor.hpp"
#include "vgui/graphics/renderpass.hpp"
#include "vgui/graphics/framebuffer.hpp"

namespace vgui {

/**
 * @brief Command buffer level
 */
enum class CommandBufferLevel {
    Primary = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    Secondary = VK_COMMAND_BUFFER_LEVEL_SECONDARY
};

/**
 * @brief Command buffer usage flags
 */
enum class CommandBufferUsage {
    OneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    RenderPassContinue = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
    SimultaneousUse = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    ConditionalRendering = VK_COMMAND_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT
};

/**
 * @brief Command buffer state
 */
enum class CommandBufferState {
    Initial,
    Recording,
    Executable,
    Pending,
    Invalid
};

/**
 * @brief Command buffer class
 * 
 * Encapsulates Vulkan command buffer recording and submission.
 */
class CommandBuffer {
public:
    /**
     * @brief Create command buffer
     */
    CommandBuffer();
    
    /**
     * @brief Create from pool
     */
    CommandBuffer(class CommandPool& pool,
                  CommandBufferLevel level = CommandBufferLevel::Primary,
                  CommandBufferUsage usage = CommandBufferUsage::OneTimeSubmit);
    
    /**
     * @brief Destroy
     */
    ~CommandBuffer();
    
    /**
     * @brief Reset command buffer
     */
    void reset(bool releaseResources = false);
    
    /**
     * @brief Begin recording
     */
    bool begin(VkCommandBufferUsageFlags usage = 0);
    
    /**
     * @brief Begin with render pass
     */
    bool beginRenderPass(VkFramebuffer framebuffer,
                         const VkClearValue* clearValues,
                         uint32_t clearValueCount,
                         VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
    
    /**
     * @brief End render pass
     */
    void endRenderPass();
    
    /**
     * @brief End recording
     */
    void end();
    
    /**
     * @brief Check if recording
     */
    bool isRecording() const { return state_ == CommandBufferState::Recording; }
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_ && handle_ != VK_NULL_HANDLE; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkCommandBuffer getHandle() const { return handle_; }
    
    /**
     * @brief Get command pool
     */
    class CommandPool* getPool() const { return pool_; }
    
    // State queries
    CommandBufferState getState() const { return state_; }
    CommandBufferLevel getLevel() const { return level_; }
    
    // Binding commands
    void bindPipeline(VkPipelineBindPoint bindPoint, VkPipeline pipeline);
    void bindVertexBuffers(uint32_t firstBinding,
                          const std::vector<VkBuffer>& buffers,
                          const std::vector<VkDeviceSize>& offsets);
    void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, 
                         VkIndexType indexType);
    void bindDescriptorSets(VkPipelineBindPoint bindPoint,
                           VkPipelineLayout layout,
                           uint32_t firstSet,
                           const std::vector<VkDescriptorSet>& sets,
                           uint32_t dynamicOffsetCount = 0,
                           const uint32_t* dynamicOffsets = nullptr);
    
    // Drawing commands
    void draw(uint32_t vertexCount, uint32_t instanceCount,
             uint32_t firstVertex, uint32_t firstInstance);
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, int32_t vertexOffset,
                     uint32_t firstInstance);
    void drawIndirect(VkBuffer buffer, VkDeviceSize offset,
                     uint32_t count, uint32_t stride);
    void drawIndexedIndirect(VkBuffer buffer, VkDeviceSize offset,
                            uint32_t count, uint32_t stride);
    
    // Viewport/scissor
    void setViewport(const VkViewport& viewport);
    void setViewports(uint32_t firstViewport, const std::vector<VkViewport>& viewports);
    void setScissor(const VkRect2D& scissor);
    void setScissors(uint32_t firstScissor, const std::vector<VkRect2D>& scissors);
    
    // Rasterizer state
    void setLineWidth(float lineWidth);
    
    // Depth/stencil
    void setDepthTestEnable(bool enable);
    void setDepthWriteEnable(bool enable);
    void setDepthCompareOp(VkCompareOp compareOp);
    void setStencilTestEnable(bool enable);
    
    // Blending
    void setBlendEnable(uint32_t attachmentIndex, bool enable);
    
    // Resources
    void pipelineBarrier(VkPipelineStageFlags srcStageMask,
                         VkPipelineStageFlags dstStageMask,
                         VkDependencyFlags dependencyFlags,
                         uint32_t memoryBarrierCount,
                         const VkMemoryBarrier* pMemoryBarriers,
                         uint32_t bufferMemoryBarrierCount,
                         const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                         uint32_t imageMemoryBarrierCount,
                         const VkImageMemoryBarrier* pImageMemoryBarriers);
    
    void imageMemoryBarrier(VkImage image,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout,
                           VkAccessFlagBits srcAccessMask,
                           VkAccessFlagBits dstAccessMask,
                           uint32_t srcQueueFamilyIndex,
                           uint32_t dstQueueFamilyIndex,
                           VkImageSubresourceRange subresourceRange);
    
    // Clears
    void clearColorImage(VkImage image, VkImageLayout layout,
                        const VkClearColorValue& clearValue,
                        uint32_t rangeCount,
                        const VkImageSubresourceRange* pRanges);
    
    void clearDepthStencilImage(VkImage image, VkImageLayout layout,
                                uint32_t stencil,
                                const VkClearDepthStencilValue& clearValue,
                                uint32_t rangeCount,
                                const VkImageSubresourceRange* pRanges);
    
    void clearAttachments(uint32_t attachmentCount,
                         const VkClearAttachment* pAttachments,
                         const VkClearRect* pRects,
                         uint32_t rectCount);
    
    // Copy operations
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
                   uint32_t regionCount,
                   const VkBufferCopy* pRegions);
    
    void copyBufferToImage(VkBuffer srcBuffer, VkImage dstImage,
                           VkImageLayout dstImageLayout,
                           uint32_t regionCount,
                           const VkBufferImageCopy* pRegions);
    
    void copyImage(VkImage srcImage, VkImageLayout srcImageLayout,
                  VkImage dstImage, VkImageLayout dstImageLayout,
                  uint32_t regionCount,
                  const VkImageCopy* pRegions);
    
    void copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout,
                           VkBuffer dstBuffer,
                           uint32_t regionCount,
                           const VkBufferImageCopy* pRegions);
    
    void blitImage(VkImage srcImage, VkImageLayout srcImageLayout,
                  VkImage dstImage, VkImageLayout dstImageLayout,
                  uint32_t regionCount,
                  const VkImageBlit* pRegions,
                  VkFilter filter);
    
    // Resolve
    void resolveImage(VkImage srcImage, VkImageLayout srcImageLayout,
                     VkImage dstImage, VkImageLayout dstImageLayout,
                     uint32_t regionCount,
                     const VkImageResolve* pRegions,
                     VkFilter filter);
    
    // Push constants
    void pushConstants(VkPipelineLayout layout,
                      VkShaderStageFlags stageFlags,
                      uint32_t offset,
                      uint32_t size,
                      const void* values);
    
    // Dispatch compute
    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
    void dispatchIndirect(VkBuffer buffer, VkDeviceSize offset);
    
    // Execution
    void submit(uint32_t waitSemaphoreCount,
                const VkSemaphore* pWaitSemaphores,
                const VkPipelineStageFlags* pWaitDstStageMask,
                uint32_t signalSemaphoreCount,
                const VkSemaphore* pSignalSemaphores,
                VkFence fence);
    
    // Utility
    template<typename T>
    void pushConstant(const std::string& name, const T& value) {
        // Placeholder for push constant helper
        // In real implementation, would need reflection data
    }
    
private:
    VkCommandBuffer handle_ = VK_NULL_HANDLE;
    CommandPool* pool_ = nullptr;
    CommandBufferLevel level_ = CommandBufferLevel::Primary;
    CommandBufferState state_ = CommandBufferState::Initial;
    bool valid_ = false;
    
    void initializeHandle();
    void resetHandle();
};

/**
 * @brief Command pool
 */
class CommandPool {
public:
    /**
     * @brief Create command pool
     */
    CommandPool();
    
    /**
     * @brief Create with queue family
     */
    CommandPool(uint32_t queueFamilyIndex,
                VkCommandPoolCreateFlags flags = 0);
    
    /**
     * @brief Destroy
     */
    ~CommandPool();
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkCommandPool getHandle() const { return pool_; }
    
    /**
     * @brief Get queue family index
     */
    uint32_t getQueueFamilyIndex() const { return queueFamilyIndex_; }
    
    /**
     * @brief Allocate command buffer
     */
    CommandBuffer allocate(CommandBufferLevel level = CommandBufferLevel::Primary,
                          CommandBufferUsage usage = CommandBufferUsage::OneTimeSubmit);
    
    /**
     * @brief Allocate multiple
     */
    std::vector<CommandBuffer> allocate(uint32_t count,
                                        CommandBufferLevel level,
                                        CommandBufferUsage usage);
    
    /**
     * @brief Free command buffer
     */
    void free(CommandBuffer& commandBuffer);
    
    /**
     * @brief Free multiple
     */
    void free(std::vector<CommandBuffer>& commandBuffers);
    
    /**
     * @brief Reset pool
     */
    void reset(VkCommandPoolResetFlags flags = 0);
    
private:
    bool create(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);
    void destroy();
    
    VkCommandPool pool_ = VK_NULL_HANDLE;
    uint32_t queueFamilyIndex_ = 0;
    bool valid_ = false;
};

/**
 * @brief Command buffer manager
 * 
 * Manages pools and command buffers for efficient recording.
 */
class CommandBufferManager {
public:
    /**
     * @brief Get instance
     */
    static CommandBufferManager& getInstance();
    
    /**
     * @brief Create pool
     */
    std::shared_ptr<CommandPool> createPool(uint32_t queueFamilyIndex,
                                            VkCommandPoolCreateFlags flags = 0);
    
    /**
     * @brief Get primary command pool
     */
    std::shared_ptr<CommandPool> getPrimaryPool();
    
    /**
     * @brief Allocate primary command buffer
     */
    CommandBuffer allocatePrimary();
    
    /**
     * @brief Reset and begin recording
     */
    CommandBuffer beginRecording();
    
    /**
     * @brief End and submit
     */
    void endAndSubmit(CommandBuffer& cmdBuffer,
                      VkSemaphore waitSemaphore = VK_NULL_HANDLE,
                      VkSemaphore signalSemaphore = VK_NULL_HANDLE);
    
private:
    CommandBufferManager() = default;
    
    std::shared_ptr<CommandPool> primaryPool_;
    std::vector<std::shared_ptr<CommandPool>> pools_;
};

} // namespace vgui
