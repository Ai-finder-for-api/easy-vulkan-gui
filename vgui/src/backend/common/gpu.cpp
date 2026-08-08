#include "vgui/backend/common/gpu.hpp"

#include <iostream>

namespace vgui {
namespace backend {

GPU::GPU() {
    // Initialize GPU info
}

GPU::~GPU() {
    // Cleanup
}

bool GPU::initialize(VkInstance instance, VkPhysicalDevice physicalDevice) {
    physicalDevice_ = physicalDevice;
    
    // Get properties
    vkGetPhysicalDeviceProperties(physicalDevice_, &properties_);
    vkGetPhysicalDeviceFeatures(physicalDevice_, &features_);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memoryProperties_);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount_, 
                                              nullptr);
    
    queueFamilyProperties_.resize(queueFamilyCount_);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount_, 
                                             queueFamilyProperties_.data());
    
    // Find queue families
    findQueueFamilies();
    
    return true;
}

void GPU::shutdown() {
    physicalDevice_ = VK_NULL_HANDLE;
    queueFamilyProperties_.clear();
}

VkPhysicalDevice GPU::getPhysicalDevice() const {
    return physicalDevice_;
}

const VkPhysicalDeviceProperties& GPU::getProperties() const {
    return properties_;
}

const VkPhysicalDeviceFeatures& GPU::getFeatures() const {
    return features_;
}

const VkPhysicalDeviceMemoryProperties& GPU::getMemoryProperties() const {
    return memoryProperties_;
}

VkDeviceSize GPU::getMaxMemoryAllocationSize() const {
    return properties_.limits.maxMemoryAllocationSize;
}

uint32_t GPU::getMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const {
    for (uint32_t i = 0; i < memoryProperties_.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memoryProperties_.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return UINT32_MAX;
}

bool GPU::hasExtension(const char* extension) const {
    for (const char* ext : supportedExtensions_) {
        if (strcmp(ext, extension) == 0) {
            return true;
        }
    }
    return false;
}

uint32_t GPU::getQueueFamilyIndex(QueueFamilyType type) const {
    switch (type) {
        case QueueFamilyType::Graphics:
            return graphicsQueueFamilyIndex_;
        case QueueFamilyType::Compute:
            return computeQueueFamilyIndex_;
        case QueueFamilyType::Transfer:
            return transferQueueFamilyIndex_;
        case QueueFamilyType::Present:
            return presentQueueFamilyIndex_;
    }
    return UINT32_MAX;
}

void GPU::findQueueFamilies() {
    uint32_t graphicsCount = 0;
    uint32_t presentCount = 0;
    uint32_t computeCount = 0;
    uint32_t transferCount = 0;
    
    for (uint32_t i = 0; i < queueFamilyProperties_.size(); i++) {
        const auto& queueFamily = queueFamilyProperties_[i];
        
        // Graphics queue
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            if (graphicsQueueFamilyIndex_ == UINT32_MAX) {
                graphicsQueueFamilyIndex_ = i;
            }
            graphicsCount++;
        }
        
        // Present queue
        // Would check surface support here
        
        // Compute queue
        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            if (computeQueueFamilyIndex_ == UINT32_MAX) {
                computeQueueFamilyIndex_ = i;
            }
            computeCount++;
        }
        
        // Transfer queue (exclusive)
        if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
            (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0 &&
            (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)) {
            if (transferQueueFamilyIndex_ == UINT32_MAX) {
                transferQueueFamilyIndex_ = i;
            }
            transferCount++;
        }
    }
    
    // If no dedicated transfer queue, use graphics queue
    if (transferQueueFamilyIndex_ == UINT32_MAX && graphicsQueueFamilyIndex_ != UINT32_MAX) {
        transferQueueFamilyIndex_ = graphicsQueueFamilyIndex_;
    }
    
    // Log findings
    std::cout << "GPU Queue Families:" << std::endl;
    std::cout << "  Graphics: " << (graphicsQueueFamilyIndex_ != UINT32_MAX ? 
                             std::to_string(graphicsQueueFamilyIndex_) : "none") << std::endl;
    std::cout << "  Compute: " << (computeQueueFamilyIndex_ != UINT32_MAX ? 
                            std::to_string(computeQueueFamilyIndex_) : "none") << std::endl;
    std::cout << "  Transfer: " << (transferQueueFamilyIndex_ != UINT32_MAX ? 
                              std::to_string(transferQueueFamilyIndex_) : "none") << std::endl;
}

} // namespace backend
} // namespace vgui
