#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <array>

#ifdef VGUI_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(VGUI_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>

#include "vgui/graphics/vertex.hpp"
#include "vgui/graphics/buffer.hpp"
#include "vgui/graphics/material.hpp"
#include "vgui/math/vec3.hpp"
#include "vgui/math/vec2.hpp"
#include "vgui/math/quat.hpp"

namespace vgui {

/**
 * @brief Per-vertex data for general meshes
 */
struct Vertex {
    Vec3f position;
    Vec3f normal;
    Vec2f uv;
    Colorf color;
    
    Vertex() = default;
    Vertex(Vec3f pos, Vec3f n, Vec2f uvCoord, Colorf c = Colorf::WHITE)
        : position(pos), normal(n), uv(uvCoord), color(c) {}
    
    // Vertex buffer layout
    static constexpr std::array<VertexElement, 4> getElements() {
        return {{
            {0, 0, VertexAttributeType::Float3, offsetof(Vertex, position), 0, false, "position"},
            {0, 1, VertexAttributeType::Float3, offsetof(Vertex, normal), 0, false, "normal"},
            {0, 2, VertexAttributeType::Float2, offsetof(Vertex, uv), 0, false, "uv"},
            {0, 3, VertexAttributeType::Float4, offsetof(Vertex, color), 0, false, "color"}
        }};
    }
    
    static constexpr uint32_t getStride() {
        return sizeof(Vertex);
    }
};

/**
 * @brief Mesh primitive type
 */
enum class MeshPrimitive {
    Triangles,
    Lines,
    Points
};

/**
 * @brief Mesh flags
 */
enum class MeshFlag {
    None = 0,
    Static = 1 << 0,         // Won't be updated frequently
    Dynamic = 1 << 1,        // Will be updated frequently
    Instanced = 1 << 2,      // Supports instanced rendering
    Skinned = 1 << 3,        // Has bone weights
    ShadowCaster = 1 << 4,   // Used for shadow rendering
    NoCull = 1 << 5,         // Disable culling
};

/**
 * @brief Mesh class
 * 
 * Represents a renderable mesh with vertices, indices, and optional
 * material. Supports static and dynamic meshes with instancing.
 */
class Mesh {
public:
    /**
     * @brief Create an empty mesh
     */
    Mesh();
    
    /**
     * @brief Create mesh from vertex/index arrays
     */
    Mesh(const std::vector<Vertex>& vertices,
         const std::vector<uint32_t>& indices,
         const std::shared_ptr<Material>& material = nullptr);
    
    /**
     * @brief Destroy the mesh
     */
    ~Mesh();
    
    /**
     * @brief Check if mesh is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Set vertices
     */
    void setVertices(const std::vector<Vertex>& vertices);
    
    /**
     * @brief Set indices
     */
    void setIndices(const std::vector<uint32_t>& indices);
    
    /**
     * @brief Set material
     */
    void setMaterial(std::shared_ptr<Material> material);
    
    /**
     * @brief Get material
     */
    std::shared_ptr<Material> getMaterial() const { return material_; }
    
    /**
     * @brief Get vertex count
     */
    uint32_t getVertexCount() const { return vertexCount_; }
    
    /**
     * @brief Get index count
     */
    uint32_t getIndexCount() const { return indexCount_; }
    
    /**
     * @brief Get vertex buffer
     */
    const VertexBuffer& getVertexBuffer() const { return *vertexBuffer_; }
    
    /**
     * @brief Get index buffer
     */
    const IndexBuffer& getIndexBuffer() const { return *indexBuffer_; }
    
    /**
     * @brief Get bounding box
     */
    const BoundingBox& getBounds() const { return bounds_; }
    
    /**
     * @brief Compute bounds from vertices
     */
    void computeBounds();
    
    /**
     * @brief Check if mesh has indices
     */
    bool hasIndices() const { return indexCount_ > 0; }
    
    /**
     * @brief Draw mesh
     */
    void draw(class CommandBuffer& cmdBuffer,
              uint32_t instanceCount = 1,
              uint32_t firstInstance = 0) const;
    
    /**
     * @brief Draw mesh directly to command buffer
     */
    static void draw(CommandBuffer& cmdBuffer,
                     const VertexBuffer& vbo,
                     const IndexBuffer& ibo,
                     uint32_t instanceCount = 1);
    
    /**
     * @brief Create primitive shapes
     */
    static std::shared_ptr<Mesh> createPlane(float width = 1.0f, float height = 1.0f,
                                              uint32_t segmentsX = 1, uint32_t segmentsY = 1);
    
    static std::shared_ptr<Mesh> createBox(float width = 1.0f, float height = 1.0f, 
                                            float depth = 1.0f);
    
    static std::shared_ptr<Mesh> createSphere(float radius = 0.5f, 
                                               uint32_t segments = 16, 
                                               uint32_t rings = 12);
    
    static std::shared_ptr<Mesh> createCylinder(float radius = 0.5f, float height = 1.0f,
                                                 uint32_t segments = 16);
    
    static std::shared_ptr<Mesh> createTriangle();
    
    static std::shared_ptr<Mesh> createCubeMapFace();
    
    /**
     * @brief Create mesh from vertex format
     */
    template<typename TVertex>
    static std::shared_ptr<Mesh> createFromTyped(const std::vector<TVertex>& vertices,
                                                  const std::vector<uint32_t>& indices) {
        std::vector<Vertex> verts(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            // Convert from typed vertex to standard Vertex
            // This requires the typed vertex to have compatible layout
            const auto& src = vertices[i];
            verts[i].position = src.position;
            verts[i].normal = src.normal;
            verts[i].uv = src.uv;
            verts[i].color = src.color;
        }
        
        auto mesh = std::make_shared<Mesh>();
        mesh->setVertices(verts);
        if (!indices.empty()) {
            mesh->setIndices(indices);
        }
        return mesh;
    }
    
    /**
     * @brief Get mesh name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set mesh name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Get mesh flags
     */
    uint32_t getFlags() const { return flags_; }
    
private:
    void createBuffers();
    void updateBuffers();
    
    bool valid_ = false;
    std::string name_;
    
    std::vector<Vertex> vertices_;
    std::vector<uint32_t> indices_;
    uint32_t vertexCount_ = 0;
    uint32_t indexCount_ = 0;
    
    std::shared_ptr<VertexBuffer> vertexBuffer_;
    std::shared_ptr<IndexBuffer> indexBuffer_;
    std::shared_ptr<Material> material_;
    
    BoundingBox bounds_;
    uint32_t flags_ = static_cast<uint32_t>(MeshFlag::Static);
};

/**
 * @brief Aabb bounding box
 */
struct BoundingBox {
    Vec3f min = Vec3f::ZERO;
    Vec3f max = Vec3f::ZERO;
    Vec3f center() const { return (min + max) * 0.5f; }
    Vec3f extent() const { return (max - min) * 0.5f; }
    float radius() const { 
        Vec3f e = extent();
        return std::sqrt(e.x * e.x + e.y * e.y + e.z * e.z);
    }
    bool intersects(const BoundingBox& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }
    bool contains(const Vec3f& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }
};

/**
 * @brief Mesh manager for loading and caching
 */
class MeshManager {
public:
    /**
     * @brief Get instance
     */
    static MeshManager& getInstance();
    
    /**
     * @brief Load mesh from file
     */
    std::shared_ptr<Mesh> load(const std::string& path);
    
    /**
     * @brief Get cached mesh
     */
    std::shared_ptr<Mesh> get(const std::string& name);
    
    /**
     * @brief Add mesh to cache
     */
    void add(const std::string& name, std::shared_ptr<Mesh> mesh);
    
    /**
     * @brief Check if mesh exists
     */
    bool has(const std::string& name) const;
    
    /**
     * @brief Clear cache
     */
    void clear();
    
private:
    MeshManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes_;
};

} // namespace vgui
