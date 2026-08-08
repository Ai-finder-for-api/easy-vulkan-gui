#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include "vgui/math/vec2.hpp"
#include "vgui/math/vec3.hpp"
#include "vgui/math/vec4.hpp"

namespace vgui {

/**
 * @brief Vertex attribute types
 */
enum class VertexAttributeType : uint32_t {
    Float = 0,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Uint,
    Uint2,
    Uint3,
    Uint4,
    Short,
    UShort
};

/**
 * @brief Vertex element descriptor
 */
struct VertexElement {
    uint32_t binding = 0;
    uint32_t location = 0;
    VertexAttributeType type = VertexAttributeType::Float;
    uint32_t offset = 0;
    uint32_t divisor = 0;  // For instanced rendering
    bool normalized = false;
    const char* name = nullptr;
    
    static uint32_t getSize(VertexAttributeType type) {
        switch (type) {
            case VertexAttributeType::Float: return sizeof(float);
            case VertexAttributeType::Float2: return sizeof(float) * 2;
            case VertexAttributeType::Float3: return sizeof(float) * 3;
            case VertexAttributeType::Float4: return sizeof(float) * 4;
            case VertexAttributeType::Int: return sizeof(int32_t);
            case VertexAttributeType::Int2: return sizeof(int32_t) * 2;
            case VertexAttributeType::Int3: return sizeof(int32_t) * 3;
            case VertexAttributeType::Int4: return sizeof(int32_t) * 4;
            case VertexAttributeType::Uint: return sizeof(uint32_t);
            case VertexAttributeType::Uint2: return sizeof(uint32_t) * 2;
            case VertexAttributeType::Uint3: return sizeof(uint32_t) * 3;
            case VertexAttributeType::Uint4: return sizeof(uint32_t) * 4;
            case VertexAttributeType::Short: return sizeof(int16_t);
            case VertexAttributeType::UShort: return sizeof(uint16_t);
        }
        return 0;
    }
};

/**
 * @brief Standard vertex formats
 */
namespace vertex {

/**
 * @brief Position-only vertex
 */
struct Position {
    Vec3f position;
    static constexpr std::array<VertexElement, 1> elements = {{
        {0, 0, VertexAttributeType::Float3, offsetof(Position, position), 0, false, "position"}
    }};
    static constexpr uint32_t stride = sizeof(Position);
};

/**
 * @brief Position and color vertex
 */
struct PositionColor {
    Vec3f position;
    Colorf color;
    static constexpr std::array<VertexElement, 2> elements = {{
        {0, 0, VertexAttributeType::Float3, offsetof(PositionColor, position), 0, false, "position"},
        {0, 1, VertexAttributeType::Float4, offsetof(PositionColor, color), 0, false, "color"}
    }};
    static constexpr uint32_t stride = sizeof(PositionColor);
};

/**
 * @brief Position and UV vertex (for textured geometry)
 */
struct PositionUV {
    Vec3f position;
    Vec2f uv;
    static constexpr std::array<VertexElement, 2> elements = {{
        {0, 0, VertexAttributeType::Float3, offsetof(PositionUV, position), 0, false, "position"},
        {0, 1, VertexAttributeType::Float2, offsetof(PositionUV, uv), 0, false, "uv"}
    }};
    static constexpr uint32_t stride = sizeof(PositionUV);
};

/**
 * @brief Position, normal, and UV vertex (for lit geometry)
 */
struct PositionNormalUV {
    Vec3f position;
    Vec3f normal;
    Vec2f uv;
    static constexpr std::array<VertexElement, 3> elements = {{
        {0, 0, VertexAttributeType::Float3, offsetof(PositionNormalUV, position), 0, false, "position"},
        {0, 1, VertexAttributeType::Float3, offsetof(PositionNormalUV, normal), 0, false, "normal"},
        {0, 2, VertexAttributeType::Float2, offsetof(PositionNormalUV, uv), 0, false, "uv"}
    }};
    static constexpr uint32_t stride = sizeof(PositionNormalUV);
};

/**
 * @brief Full PBR vertex (position, normal, tangent, UV)
 */
struct PBRVertex {
    Vec3f position;
    Vec3f normal;
    Vec4f tangent;   // xyz = tangent, w = handedness
    Vec2f uv;
    static constexpr std::array<VertexElement, 4> elements = {{
        {0, 0, VertexAttributeType::Float3, offsetof(PBRVertex, position), 0, false, "position"},
        {0, 1, VertexAttributeType::Float3, offsetof(PBRVertex, normal), 0, false, "normal"},
        {0, 2, VertexAttributeType::Float4, offsetof(PBRVertex, tangent), 0, false, "tangent"},
        {0, 3, VertexAttributeType::Float2, offsetof(PBRVertex, uv), 0, false, "uv"}
    }};
    static constexpr uint32_t stride = sizeof(PBRVertex);
};

/**
 * @brief Text rendering vertex (position, UV, color)
 */
struct TextVertex {
    Vec3f position;
    Vec2f uv;
    Colorf color;
    static constexpr std::array<VertexElement, 3> elements = {{
        {0, 0, VertexAttributeType::Float3, offsetof(TextVertex, position), 0, false, "position"},
        {0, 1, VertexAttributeType::Float2, offsetof(TextVertex, uv), 0, false, "uv"},
        {0, 2, VertexAttributeType::Float4, offsetof(TextVertex, color), 0, false, "color"}
    }};
    static constexpr uint32_t stride = sizeof(TextVertex);
};

/**
 * @brief UI vertex (position, color, UV)
 */
struct UIVertex {
    Vec2f position;
    Vec4f color;
    Vec2f uv;
    static constexpr std::array<VertexElement, 3> elements = {{
        {0, 0, VertexAttributeType::Float2, offsetof(UIVertex, position), 0, false, "position"},
        {0, 1, VertexAttributeType::Float4, offsetof(UIVertex, color), 0, false, "color"},
        {0, 2, VertexAttributeType::Float2, offsetof(UIVertex, uv), 0, false, "uv"}
    }};
    static constexpr uint32_t stride = sizeof(UIVertex);
};

} // namespace vertex

/**
 * @brief Vertex buffer class
 * 
 * Stores vertex data on the GPU for efficient rendering.
 */
class VertexBuffer {
public:
    /**
     * @brief Create a vertex buffer
     * @param data Pointer to vertex data
     * @param size Size in bytes
     * @param usage Usage flags (static, dynamic, etc.)
     */
    VertexBuffer(const void* data, uint32_t size, 
                 uint32_t usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    
    /**
     * @brief Create from vertex array
     * @tparam TVertex Vertex type
     * @param vertices Array of vertices
     */
    template<typename TVertex>
    static VertexBuffer fromArray(const std::vector<TVertex>& vertices) {
        return VertexBuffer(
            vertices.data(),
            static_cast<uint32_t>(vertices.size() * sizeof(TVertex)),
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        );
    }
    
    /**
     * @brief Destroy the buffer
     */
    ~VertexBuffer();
    
    /**
     * @brief Check if buffer is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get buffer handle
     */
    void* getHandle() const { return handle_; }
    
    /**
     * @brief Get buffer size
     */
    uint32_t getSize() const { return size_; }
    
    /**
     * @brief Update buffer data
     * @param data New data
     * @param size Size in bytes
     * @param offset Offset within buffer
     */
    void update(const void* data, uint32_t size, uint32_t offset = 0);
    
private:
    bool valid_ = false;
    void* handle_ = nullptr;
    uint32_t size_ = 0;
    uint32_t usage_ = 0;
};

/**
 * @brief Index buffer class
 */
class IndexBuffer {
public:
    /**
     * @brief Create an index buffer
     * @param indices Index data
     * @param count Number of indices
     * @param indexType Index type (16-bit or 32-bit)
     */
    IndexBuffer(const void* indices, uint32_t count, 
                uint32_t indexType = VK_INDEX_TYPE_UINT32);
    
    /**
     * @brief Create from index array
     */
    template<typename TIndex>
    static IndexBuffer fromArray(const std::vector<TIndex>& indices) {
        uint32_t indexType = sizeof(TIndex) == 2 ? 
                              VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
        return IndexBuffer(
            indices.data(),
            static_cast<uint32_t>(indices.size()),
            indexType
        );
    }
    
    /**
     * @brief Destroy the buffer
     */
    ~IndexBuffer();
    
    /**
     * @brief Check if buffer is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get buffer handle
     */
    void* getHandle() const { return handle_; }
    
    /**
     * @brief Get index count
     */
    uint32_t getCount() const { return count_; }
    
    /**
     * @brief Get index type
     */
    uint32_t getIndexType() const { return indexType_; }
    
private:
    bool valid_ = false;
    void* handle_ = nullptr;
    uint32_t count_ = 0;
    uint32_t indexType_ = 0;
};

/**
 * @brief Vertex Array Object (VAO)
 * 
 * Encapsulates vertex buffer bindings and attribute configurations.
 */
class VertexArray {
public:
    /**
     * @brief Create a vertex array
     */
    VertexArray();
    
    /**
     * @brief Destroy the VAO
     */
    ~VertexArray();
    
    /**
     * @brief Bind the VAO
     */
    void bind() const;
    
    /**
     * @brief Unbind the VAO
     */
    static void unbind();
    
    /**
     * @brief Add a vertex buffer binding
     */
    void addVertexBuffer(VertexBuffer& buffer, 
                         const std::vector<VertexElement>& elements);
    
    /**
     * @brief Set the index buffer
     */
    void setIndexBuffer(IndexBuffer& buffer);
    
    /**
     * @brief Draw primitives
     */
    void draw(uint32_t vertexCount, uint32_t instanceCount = 1,
             uint32_t firstVertex = 0, uint32_t firstInstance = 0) const;
    
    /**
     * @brief Draw indexed primitives
     */
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1,
                     uint32_t firstIndex = 0, int32_t vertexOffset = 0,
                     uint32_t firstInstance = 0) const;
    
private:
    bool valid_ = false;
    void* handle_ = nullptr;
    uint32_t vertexBufferCount_ = 0;
    uint32_t indexBuffer_ = 0;
    uint32_t indexType_ = 0;
};

} // namespace vgui
