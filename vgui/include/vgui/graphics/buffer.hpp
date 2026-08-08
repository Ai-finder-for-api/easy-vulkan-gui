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

namespace vgui {

/**
 * @brief Buffer usage flags
 */
enum class BufferUsage : uint32_t {
    Vertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
    Index = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    Uniform = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    Storage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
    Indirect = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
    TransformFeedback = VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT,
    TransferSrc = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    TransferDst = VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    ShaderBindingTable = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_EXT
};

/**
 * @brief Memory property flags
 */
enum class MemoryProperty : uint32_t {
    DeviceLocal = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    HostVisible = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
    HostCoherent = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    HostCached = VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
    LazilyAllocated = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
    Protected = VK_MEMORY_PROPERTY_PROTECTED_BIT
};

/**
 * @brief Buffer creation flags
 */
enum class BufferFlag {
    None = 0,
    MapAtCreation = 1 << 0,       // Create with host-visible memory
    PersistentlyMapped = 1 << 1,  // Keep mapping after creation
    AllowTransfer = 1 << 2,       // Allow buffer-to-buffer transfers
};

/**
 * @brief Unified buffer class
 * 
 * Manages GPU buffers with support for various usage patterns.
 * Handles memory allocation, mapping, and transfer operations.
 */
class Buffer {
public:
    /**
     * @brief Create an empty buffer
     */
    Buffer();
    
    /**
     * @brief Create a buffer with data
     * @param data Initial data
     * @param size Size in bytes
     * @param usage Buffer usage flags
     * @param memoryProperties Memory property flags
     * @param flags Creation flags
     */
    Buffer(const void* data, uint32_t size,
           uint32_t usage = static_cast<uint32_t>(BufferUsage::Uniform),
           uint32_t memoryProperties = static_cast<uint32_t>(MemoryProperty::DeviceLocal),
           uint32_t flags = BufferFlag::None);
    
    /**
     * @brief Create from vector
     */
    template<typename T>
    static Buffer fromVector(const std::vector<T>& data,
                             uint32_t usage = static_cast<uint32_t>(BufferUsage::Vertex)) {
        return Buffer(
            data.data(),
            static_cast<uint32_t>(data.size() * sizeof(T)),
            usage
        );
    }
    
    /**
     * @brief Destroy the buffer
     */
    ~Buffer();
    
    // No copying
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    
    // Moving
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;
    
    /**
     * @brief Check if buffer is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan buffer handle
     */
    VkBuffer getHandle() const { return buffer_; }
    
    /**
     * @brief Get Vulkan memory handle
     */
    VkDeviceMemory getMemory() const { return memory_; }
    
    /**
     * @brief Get buffer size
     */
    uint32_t getSize() const { return size_; }
    
    /**
     * @brief Get memory properties
     */
    uint32_t getMemoryProperties() const { return memoryProperties_; }
    
    /**
     * @brief Check if buffer is host-visible
     */
    bool isHostVisible() const {
        return (memoryProperties_ & static_cast<uint32_t>(MemoryProperty::HostVisible)) != 0;
    }
    
    /**
     * @brief Map buffer memory
     * @param offset Offset within buffer
     * @param size Size to map (0 = entire buffer)
     * @return Pointer to mapped memory, or nullptr on failure
     */
    void* map(uint32_t offset = 0, uint32_t size = 0);
    
    /**
     * @brief Unmap buffer memory
     */
    void unmap();
    
    /**
     * @brief Flush mapped memory ranges (for non-coherent memory)
     */
    void flush(uint32_t offset = 0, uint32_t size = 0);
    
    /**
     * @brief Invalidate mapped memory ranges (for device-local memory imported)
     */
    void invalidate(uint32_t offset = 0, uint32_t size = 0);
    
    /**
     * @brief Update buffer data
     * @param data New data
     * @param size Size in bytes
     * @param offset Offset within buffer
     * 
     * Uses staging buffer for device-local memory.
     */
    void update(const void* data, uint32_t size, uint32_t offset = 0);
    
    /**
     * @brief Upload data via staging buffer
     * @param data Source data
     * @param size Size in bytes
     */
    void upload(const void* data, uint32_t size);
    
    /**
     * @brief Copy buffer to another buffer
     */
    static void copyBuffer(Buffer& src, Buffer& dst, uint32_t srcOffset = 0,
                          uint32_t dstOffset = 0, uint32_t size = 0);
    
    /**
     * @brief Create a readback buffer for CPU access to GPU data
     */
    static Buffer createReadback(uint32_t size);
    
    /**
     * @brief Get mapped pointer (if persistently mapped)
     */
    void* getMappedPointer() const { return mappedPointer_; }
    
    /**
     * @brief Check if buffer is mapped
     */
    bool isMapped() const { return mapped_; }
    
private:
    bool createBuffer(uint32_t size, uint32_t usage, 
                      uint32_t memoryProperties, uint32_t flags);
    void allocateMemory(uint32_t memoryTypeIndex);
    VkMemoryRequirements getRequirements() const;
    uint32_t findMemoryType(uint32_t typeFilter, 
                            uint32_t memoryProperties) const;
    
    bool valid_ = false;
    VkBuffer buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory memory_ = VK_NULL_HANDLE;
    uint32_t size_ = 0;
    uint32_t usage_ = 0;
    uint32_t memoryProperties_ = 0;
    uint32_t memoryTypeIndex_ = 0;
    
    void* mappedPointer_ = nullptr;
    bool mapped_ = false;
    bool persistentMapping_ = false;
};

/**
 * @brief Specialized uniform buffer with alignment handling
 */
class UniformBuffer : private Buffer {
public:
    /**
     * @brief Create uniform buffer
     * @param data Initial data
     * @param size Size in bytes
     * @param deviceAddress If true, buffer will be device-addressable
     */
    template<typename T>
    UniformBuffer(const T& data, bool deviceAddress = false) 
        : Buffer(&data, sizeof(T), 
                 static_cast<uint32_t>(BufferUsage::Uniform),
                 static_cast<uint32_t>(MemoryProperty::DeviceLocal)) {
        (void)deviceAddress;
    }
    
    using Buffer::isValid;
    using Buffer::getHandle;
    using Buffer::getMemory;
    using Buffer::getSize;
    using Buffer::update;
    using Buffer::map;
    using Buffer::unmap;
};

/**
 * @brief Storage buffer for compute shaders
 */
class StorageBuffer : private Buffer {
public:
    template<typename T>
    StorageBuffer(const std::vector<T>& data)
        : Buffer(data.data(), 
                 static_cast<uint32_t>(data.size() * sizeof(T)),
                 static_cast<uint32_t>(BufferUsage::Storage) |
                 static_cast<uint32_t>(BufferUsage::TransferDst)) {}
    
    using Buffer::isValid;
    using Buffer::getHandle;
    using Buffer::update;
};

/**
 * @brief Index buffer wrapper
 */
class IndexBuffer : private Buffer {
public:
    IndexBuffer(const void* indices, uint32_t count, 
                uint32_t indexType = VK_INDEX_TYPE_UINT32);
    
    uint32_t getIndexCount() const { return size_ / indexTypeSize_; }
    uint32_t getIndexType() const { return indexType_; }
    
private:
    uint32_t indexType_ = VK_INDEX_TYPE_UINT32;
    uint32_t indexTypeSize_ = 4; // Default to 32-bit
};

/**
 * @brief Vertex buffer wrapper
 */
class VertexBuffer : private Buffer {
public:
    VertexBuffer(const void* vertices, uint32_t size);
    
    using Buffer::isValid;
    using Buffer::getHandle;
};

} // namespace vgui
