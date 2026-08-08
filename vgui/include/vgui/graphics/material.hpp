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
#include "vgui/graphics/pipeline.hpp"
#include "vgui/graphics/descriptor.hpp"
#include "vgui/math/color.hpp"

namespace vgui {

/**
 * @brief Material property types
 */
enum class MaterialType {
    Unlit,           // Simple color, no lighting
    Textured,        // Texture without lighting
    Phong,           // Blinn-Phong lighting
    PBR,             // Physically based rendering
    Custom           // User-defined shader
};

/**
 * @brief Material property
 */
struct MaterialProperty {
    std::string name;
    MaterialType type = MaterialType::Unlit;
    
    // Color/value properties
    Colorf color = Colorf::WHITE;
    float scalar = 1.0f;
    int32_t integer = 0;
    
    // Texture properties
    std::shared_ptr<Texture> texture;
    bool hasTexture = false;
    
    // Mapping
    uint32_t set = 0;
    uint32_t binding = 0;
};

/**
 * @brief Material uniform block
 */
struct MaterialUniform {
    std::string name;
    uint32_t binding = 0;
    uint32_t size = 0;
    uint32_t offset = 0;
    std::vector<uint8_t> data;
};

/**
 * @brief Material class
 * 
 * Encapsulates shader, textures, and uniform data for rendering.
 * Supports multiple material types and property binding.
 */
class Material {
public:
    /**
     * @brief Create an empty material
     */
    Material();
    
    /**
     * @brief Create material with shader
     */
    Material(std::shared_ptr<class Shader> shader,
             std::shared_ptr<PipelineLayout> layout);
    
    /**
     * @brief Destroy the material
     */
    ~Material();
    
    /**
     * @brief Check if material is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Set shader
     */
    void setShader(std::shared_ptr<Shader> shader);
    
    /**
     * @brief Get shader
     */
    std::shared_ptr<Shader> getShader() const { return shader_; }
    
    /**
     * @brief Set pipeline layout
     */
    void setLayout(std::shared_ptr<PipelineLayout> layout);
    
    /**
     * @brief Get pipeline layout
     */
    std::shared_ptr<PipelineLayout> getLayout() const { return layout_; }
    
    /**
     * @brief Get material type
     */
    MaterialType getType() const { return type_; }
    
    /**
     * @brief Set material type
     */
    void setType(MaterialType type);
    
    /**
     * @brief Set color property
     */
    void setProperty(const std::string& name, const Colorf& color);
    
    /**
     * @brief Set scalar property
     */
    void setProperty(const std::string& name, float value);
    
    /**
     * @brief Set integer property
     */
    void setProperty(const std::string& name, int32_t value);
    
    /**
     * @brief Set texture property
     */
    void setProperty(const std::string& name, std::shared_ptr<Texture> texture);
    
    /**
     * @brief Get property
     */
    const MaterialProperty* getProperty(const std::string& name) const;
    
    /**
     * @brief Get all properties
     */
    const std::vector<MaterialProperty>& getProperties() const { return properties_; }
    
    /**
     * @brief Bind material to descriptor sets
     */
    void bind(class DescriptorSetManager& descriptorManager, 
              uint32_t setIndex = 0);
    
    /**
     * @brief Update uniform buffer
     */
    void updateUniforms();
    
    /**
     * @brief Get material name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set material name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Check if material is transparent
     */
    bool isTransparent() const { return transparent_; }
    
    /**
     * @brief Set transparency
     */
    void setTransparent(bool transparent) { transparent_ = transparent; }
    
    /**
     * @brief Get opacity
     */
    float getOpacity() const { return opacity_; }
    
    /**
     * @brief Set opacity
     */
    void setOpacity(float opacity) { 
        opacity_ = std::max(0.0f, std::min(1.0f, opacity));
        transparent_ = opacity_ < 1.0f;
    }
    
    /**
     * @brief Get double-sided flag
     */
    bool isDoubleSided() const { return doubleSided_; }
    
    /**
     * @brief Set double-sided
     */
    void setDoubleSided(bool doubleSided) { doubleSided_ = doubleSided; }
    
    // Common material presets
    static std::shared_ptr<Material> createUnlit(const Colorf& color);
    static std::shared_ptr<Material> createTextured(std::shared_ptr<Texture> texture);
    static std::shared_ptr<Material> createPhong(const Colorf& color,
                                                  float ambient = 0.2f,
                                                  float diffuse = 0.8f,
                                                  float specular = 0.5f,
                                                  float shininess = 32.0f);
    
private:
    void updateProperties();
    void createUniformBuffers();
    void buildDescriptorSets();
    
    bool valid_ = false;
    std::shared_ptr<Shader> shader_;
    std::shared_ptr<PipelineLayout> layout_;
    MaterialType type_ = MaterialType::Unlit;
    
    std::string name_;
    bool transparent_ = false;
    float opacity_ = 1.0f;
    bool doubleSided_ = false;
    
    std::vector<MaterialProperty> properties_;
    std::vector<MaterialUniform> uniforms_;
    
    // Resources
    std::vector<std::shared_ptr<Buffer>> uniformBuffers_;
    std::vector<std::shared_ptr<DescriptorSet>> descriptorSets_;
    
    bool needsUpdate_ = true;
};

/**
 * @brief Material instance with per-instance data
 */
class MaterialInstance {
public:
    MaterialInstance(std::shared_ptr<Material> material);
    
    /**
     * @brief Bind the material
     */
    void bind(class CommandBuffer& cmdBuffer);
    
    /**
     * @brief Set instance color
     */
    void setColor(const Colorf& color);
    
    /**
     * @brief Get the material
     */
    std::shared_ptr<Material> getMaterial() const { return material_; }
    
private:
    std::shared_ptr<Material> material_;
    Colorf instanceColor_ = Colorf::WHITE;
};

} // namespace vgui
