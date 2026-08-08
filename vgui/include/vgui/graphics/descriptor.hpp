#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#ifdef VGUI_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(VGUI_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>

#include "vgui/graphics/texture.hpp"
#include "vgui/graphics/buffer.hpp"

namespace vgui {

/**
 * @brief Descriptor type
 */
enum class DescriptorType {
    Sampler = VK_DESCRIPTOR_TYPE_SAMPLER,
    CombinedImageSampler = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    SampledImage = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    StorageImage = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    UniformBuffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    StorageBuffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    UniformBufferDynamic = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    StorageBufferDynamic = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    InputAttachment = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
};

/**
 * @brief Descriptor binding
 */
struct DescriptorBinding {
    uint32_t binding = 0;
    DescriptorType type = DescriptorType::CombinedImageSampler;
    uint32_t descriptorCount = 1;
    uint32_t stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    const char* name = nullptr;
    
    // For sampler descriptor
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    
    // For image descriptors
    bool immutableSamplers = false;
};

/**
 * @brief Descriptor set layout
 */
class DescriptorSetLayout {
public:
    /**
     * @brief Create empty layout
     */
    DescriptorSetLayout();
    
    /**
     * @brief Add binding
     */
    void addBinding(const DescriptorBinding& binding);
    
    /**
     * @brief Create the layout
     */
    bool create();
    
    /**
     * @brief Destroy
     */
    ~DescriptorSetLayout();
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkDescriptorSetLayout getHandle() const { return handle_; }
    
    /**
     * @brief Get bindings
     */
    const std::vector<DescriptorBinding>& getBindings() const { return bindings_; }
    
    /**
     * @brief Find binding by index
     */
    const DescriptorBinding* findBinding(uint32_t binding) const;
    
private:
    std::vector<DescriptorBinding> bindings_;
    VkDescriptorSetLayout handle_ = VK_NULL_HANDLE;
    bool valid_ = false;
};

/**
 * @brief Descriptor pool
 */
class DescriptorPool {
public:
    /**
     * @brief Create pool
     */
    DescriptorPool();
    
    /**
     * @brief Add pool size
     */
    void addPoolSize(DescriptorType type, uint32_t count);
    
    /**
     * @brief Set maximums
     */
    void setMaxSets(uint32_t maxSets);
    void setFlags(VkDescriptorPoolCreateFlags flags);
    
    /**
     * @brief Create the pool
     */
    bool create();
    
    /**
     * @brief Destroy
     */
    ~DescriptorPool();
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkDescriptorPool getHandle() const { return pool_; }
    
    /**
     * @brief Reset pool
     */
    void reset();
    
    /**
     * @brief Destroy pool
     */
    void destroy();
    
private:
    uint32_t maxSets_ = 0;
    VkDescriptorPoolCreateFlags flags_ = 0;
    std::vector<VkDescriptorPoolSize> poolSizes_;
    VkDescriptorPool pool_ = VK_NULL_HANDLE;
    bool valid_ = false;
};

/**
 * @brief Descriptor set
 */
class DescriptorSet {
public:
    /**
     * @brief Create descriptor set
     */
    DescriptorSet();
    
    /**
     * @brief Allocate from pool
     */
    bool allocate(DescriptorPool& pool, DescriptorSetLayout& layout);
    
    /**
     * @brief Destroy
     */
    ~DescriptorSet();
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkDescriptorSet getHandle() const { return set_; }
    
    /**
     * @brief Update buffer descriptor
     */
    void updateBuffer(uint32_t binding, Buffer& buffer,
                      uint32_t offset = 0, uint32_t size = 0,
                      uint32_t arrayIndex = 0);
    
    /**
     * @brief Update texture descriptor
     */
    void updateTexture(uint32_t binding, Texture& texture,
                       Sampler& sampler,
                       uint32_t arrayIndex = 0);
    
    /**
     * @brief Update combined image sampler
     */
    void updateCombinedImage(uint32_t binding, Texture& texture,
                             uint32_t arrayIndex = 0);
    
    /**
     * @brief Write all pending updates
     */
    void update();
    
private:
    DescriptorPool* pool_ = nullptr;
    DescriptorSetLayout* layout_ = nullptr;
    VkDescriptorSet set_ = VK_NULL_HANDLE;
    bool valid_ = false;
    
    struct PendingUpdate {
        uint32_t binding;
        VkDescriptorType type;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        std::vector<uint32_t> writeCounts;
    };
    
    std::vector<PendingUpdate> pendingUpdates_;
};

/**
 * @brief Sampler state
 */
class Sampler {
public:
    /**
     * @brief Create a sampler
     */
    Sampler();
    
    /**
     * @brief Create with configuration
     */
    Sampler(VkFilter minFilter, VkFilter magFilter,
            VkSamplerMipmapMode mipmapMode,
            VkSamplerAddressMode addressMode,
            float mipLodBias = 0.0f,
            bool anisotropyEnable = true,
            float maxAnisotropy = 16.0f);
    
    /**
     * @brief Destroy
     */
    ~Sampler();
    
    /**
     * @brief Check if valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan handle
     */
    VkSampler getHandle() const { return sampler_; }
    
    /**
     * @brief Configure sampler
     */
    void configure(VkFilter minFilter, VkFilter magFilter,
                   VkSamplerMipmapMode mipmapMode,
                   VkSamplerAddressMode addressMode,
                   float mipLodBias = 0.0f,
                   bool anisotropyEnable = true,
                   float maxAnisotropy = 16.0f);
    
    /**
     * @brief Create default UI sampler
     */
    static Sampler createUI();
    
    /**
     * @brief Create default color sampler
     */
    static Sampler createColor();
    
    /**
     * @brief Create default depth sampler
     */
    static Sampler createDepth();
    
private:
    bool create();
    void destroy();
    
    bool valid_ = false;
    VkSampler sampler_ = VK_NULL_HANDLE;
    
    VkFilter minFilter_ = VK_FILTER_LINEAR;
    VkFilter magFilter_ = VK_FILTER_LINEAR;
    VkSamplerMipmapMode mipmapMode_ = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VkSamplerAddressMode addressMode_ = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    float mipLodBias_ = 0.0f;
    bool anisotropyEnable_ = true;
    float maxAnisotropy_ = 16.0f;
};

/**
 * @brief Descriptor set manager
 * 
 * Manages allocation and binding of descriptor sets.
 */
class DescriptorSetManager {
public:
    /**
     * @brief Create manager
     */
    DescriptorSetManager();
    
    /**
     * @brief Destroy
     */
    ~DescriptorSetManager();
    
    /**
     * @brief Create a pool
     */
    std::shared_ptr<DescriptorPool> createPool(uint32_t maxSets,
                                               const std::vector<
                                                   std::pair<DescriptorType, uint32_t>>& sizes);
    
    /**
     * @brief Allocate descriptor set
     */
    std::shared_ptr<DescriptorSet> allocate(DescriptorPool& pool,
                                            DescriptorSetLayout& layout);
    
    /**
     * @brief Bind descriptor sets
     */
    static void bind(class CommandBuffer& cmdBuffer,
                     uint32_t firstSet,
                     const std::vector<std::shared_ptr<DescriptorSet>>& sets);
    
    /**
     * @brief Create default pool for materials
     */
    std::shared_ptr<DescriptorPool> createMaterialPool(uint32_t materialCount);
    
private:
    std::vector<std::shared_ptr<DescriptorPool>> pools_;
};

} // namespace vgui
