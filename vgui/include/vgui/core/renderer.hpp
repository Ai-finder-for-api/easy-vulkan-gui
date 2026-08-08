#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <optional>
#include <functional>
#include "vgui/core/config.hpp"

#ifdef VGUI_PLATFORM_MACOS
    #if VGUI_USE_METAL
        #include <Metal/Metal.hpp>
    #endif
#endif

namespace vgui {

// Forward declarations
class Window;
class Device;
class Swapchain;
class RenderPass;
class Pipeline;
class CommandBuffer;
class Framebuffer;
class Texture;
class Material;

/**
 * @brief Vulkan/Metal device wrapper
 * 
 * Manages the graphics device, queues, and memory allocator.
 */
class Device {
public:
    /**
     * @brief Create a device from configuration
     */
    static std::shared_ptr<Device> create(const Config& config);
    
    /**
     * @brief Destroy the device
     */
    ~Device();
    
    /**
     * @brief Check if device is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get the underlying device handle
     */
    void* getHandle() const { return handle_; }
    
    /**
     * @brief Get the graphics queue
     */
    void* getGraphicsQueue() const { return graphicsQueue_; }
    
    /**
     * @brief Get the present queue
     */
    void* getPresentQueue() const { return presentQueue_; }
    
    /**
     * @brief Get the compute queue (if available)
     */
    void* getComputeQueue() const { return computeQueue_; }
    
    /**
     * @brief Allocate memory
     */
    void* allocateMemory(uint64_t size, uint64_t alignment, 
                         uint32_t memoryTypeBits, 
                         uint32_t memoryPropertyFlags);
    
    /**
     * @brief Free memory
     */
    void freeMemory(void* memory);
    
    /**
     * @brief Map memory
     */
    void* mapMemory(void* memory, uint64_t offset, uint64_t size);
    
    /**
     * @brief Unmap memory
     */
    void unmapMemory(void* memory);
    
    /**
     * @brief Flush mapped memory
     */
    void flushMappedMemory(void* memory, uint64_t offset, uint64_t size);
    
    /**
     * @brief Get memory properties
     */
    struct MemoryProperties {
        uint32_t memoryTypeCount;
        struct {
            uint32_t typeBits;
            uint32_t propertyFlags;
            uint32_t heapIndex;
        } memoryTypes[32];
    };
    
    MemoryProperties getMemoryProperties() const;
    
    /**
     * @brief Create a command pool
     */
    void* createCommandPool(uint32_t queueFamilyIndex, 
                           uint32_t flags = 0);
    
    /**
     * @brief Destroy a command pool
     */
    void destroyCommandPool(void* commandPool);
    
    /**
     * @brief Allocate command buffers
     */
    std::vector<void*> allocateCommandBuffers(void* commandPool, 
                                               uint32_t count,
                                               uint32_t level);
    
    /**
     * @brief Free command buffers
     */
    void freeCommandBuffers(void* commandPool, 
                           const std::vector<void*>& commandBuffers);
    
    /**
     * @brief Create a fence
     */
    void* createFence(bool signaled = false);
    
    /**
     * @brief Destroy a fence
     */
    void destroyFence(void* fence);
    
    /**
     * @brief Wait for fence
     */
    bool waitFence(void* fence, uint64_t timeout);
    
    /**
     * @brief Reset fence
     */
    void resetFence(void* fence);
    
    /**
     * @brief Create a semaphore
     */
    void* createSemaphore();
    
    /**
     * @brief Destroy a semaphore
     */
    void destroySemaphore(void* semaphore);
    
    /**
     * @brief Create an event
     */
    void* createEvent();
    
    /**
     * @brief Destroy an event
     */
    void destroyEvent(void* event);
    
    /**
     * @brief Wait for events
     */
    void waitForEvents(uint32_t eventCount, const void** events);
    
    /**
     * @brief Set event
     */
    void setEvent(void* event);
    
    /**
     * @brief Reset event
     */
    void resetEvent(void* event);
    
    /**
     * @brief Get timestamp frequency
     */
    uint64_t getTimestampFrequency() const;
    
    /**
     * @brief Get device timestamp
     */
    uint64_t getDeviceTimestamp();
    
private:
    Device() = default;
    
    bool valid_ = false;
    void* handle_ = nullptr;
    void* graphicsQueue_ = nullptr;
    void* presentQueue_ = nullptr;
    void* computeQueue_ = nullptr;
    MemoryProperties memoryProps_;
    uint64_t timestampFrequency_ = 0;
    
    Config config_;
};

/**
 * @brief Swapchain management
 * 
 * Handles swapchain creation, recreation on resize, and image acquisition.
 */
class Swapchain {
public:
    /**
     * @brief Create a swapchain
     */
    static std::shared_ptr<Swapchain> create(Device& device, 
                                              void* surface,
                                              const Config& config);
    
    /**
     * @brief Destroy the swapchain
     */
    ~Swapchain();
    
    /**
     * @brief Check if swapchain is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get the swapchain handle
     */
    void* getHandle() const { return handle_; }
    
    /**
     * @brief Get swapchain images
     */
    const std::vector<void*>& getImages() const { return images_; }
    
    /**
     * @brief Get image views
     */
    const std::vector<void*>& getImageViews() const { return imageViews_; }
    
    /**
     * @brief Get the format
     */
    uint32_t getFormat() const { return format_; }
    
    /**
     * @brief Get the color space
     */
    uint32_t getColorSpace() const { return colorSpace_; }
    
    /**
     * @brief Get the extent
     */
    uint32_t getWidth() const { return extent_.width; }
    uint32_t getHeight() const { return extent_.height; }
    
    /**
     * @brief Get the extent struct
     */
    struct Extent2D { uint32_t width; uint32_t height; } getExtent() const { return extent_; }
    
    /**
     * @brief Get the image count
     */
    uint32_t getImageCount() const { return images_.size(); }
    
    /**
     * @brief Acquire the next image
     * @param timeout Timeout in nanoseconds (VK_TIMEOUT_NONE for no wait)
     * @return Pair of (imageIndex, result)
     */
    std::pair<uint32_t, bool> acquireNextImage(uint64_t timeout, 
                                                void* semaphore);
    
    /**
     * @brief Present the image
     * @param imageIndex Image to present
     * @param waitSemaphores Semaphores to wait on
     * @return true on success
     */
    bool present(uint32_t imageIndex, 
                const std::vector<void*>& waitSemaphores = {});
    
    /**
     * @brief Check if swapchain is out of date (needs recreation)
     */
    bool isOutOfDate() const;
    
    /**
     * @brief Recreate the swapchain with new dimensions
     */
    bool recreate(uint32_t width, uint32_t height, 
                 bool minimized = false);
    
private:
    Swapchain() = default;
    
    bool valid_ = false;
    void* handle_ = nullptr;
    Device* device_ = nullptr;
    void* surface_ = nullptr;
    
    std::vector<void*> images_;
    std::vector<void*> imageViews_;
    
    uint32_t format_ = 0;
    uint32_t colorSpace_ = 0;
    Extent2D extent_{0, 0};
    uint32_t imageCount_ = 0;
    
    Config config_;
};

} // namespace vgui
