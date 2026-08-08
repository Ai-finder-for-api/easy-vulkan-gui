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

namespace vgui {

/**
 * @brief Shader stage
 */
enum class ShaderStage : uint32_t {
    Vertex = VK_SHADER_STAGE_VERTEX_BIT,
    Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
    TessellationControl = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TessellationEvaluation = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    Compute = VK_SHADER_STAGE_COMPUTE_BIT,
    AllGraphics = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT |
                  VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT |
                  VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
};

/**
 * @brief Shader data source
 */
enum class ShaderSourceType {
    SPIRV,      // Pre-compiled SPIR-V binary
    GLSL,       // GLSL source (will be compiled)
    Metal,      // Metal shader source (macOS only)
};

/**
 * @brief Shader reflection information
 */
struct ShaderReflection {
    uint32_t stage = 0;
    std::string entryPoint = "main";
    
    struct Binding {
        uint32_t set = 0;
        uint32_t binding = 0;
        uint32_t descriptorType = 0;
        uint32_t descriptorCount = 1;
        uint32_t stageFlags = 0;
        std::string name;
        uint32_t offset = 0;      // For uniform buffers
        uint32_t size = 0;        // For uniform buffers
        uint32_t arraySize = 0;   // For arrays
        std::vector<std::string> members; // For uniform blocks
    };
    
    struct PushConstant {
        uint32_t offset = 0;
        uint32_t size = 0;
        std::string name;
    };
    
    struct Attribute {
        uint32_t location = 0;
        std::string name;
        uint32_t format = 0;
        uint32_t offset = 0;
        uint32_t shaderLocation = 0;
    };
    
    std::vector<Binding> bindings;
    std::vector<PushConstant> pushConstants;
    std::vector<Attribute> attributes;
    
    // Get binding by name
    const Binding* findBinding(const std::string& setName, 
                               uint32_t binding) const;
    
    // Get binding by name (global)
    const Binding* findBinding(uint32_t binding) const;
};

/**
 * @brief Shader class
 * 
 * Manages compiled shader modules with reflection support.
 * Supports SPIR-V, GLSL compilation, and Metal shaders.
 */
class Shader {
public:
    /**
     * @brief Create an empty shader
     */
    Shader();
    
    /**
     * @brief Create shader from SPIR-V binary
     * @param spirv SPIR-V bytecode
     * @param stage Shader stage
     * @param entryPoint Entry point function name
     */
    Shader(const void* spirv, size_t size, 
           ShaderStage stage = ShaderStage::Vertex,
           const std::string& entryPoint = "main");
    
    /**
     * @brief Create shader from file
     * @param path Path to SPIR-V file
     * @param stage Shader stage
     */
    bool loadFromFile(const std::string& path,
                      ShaderStage stage = ShaderStage::Vertex,
                      const std::string& entryPoint = "main");
    
    /**
     * @brief Create shader from memory (SPIR-V)
     */
    bool loadFromMemory(const void* data, size_t size,
                        ShaderStage stage = ShaderStage::Vertex,
                        const std::string& entryPoint = "main");
    
    /**
     * @brief Create shader from GLSL source
     * @deprecated Use a separate compiler or pre-compile to SPIR-V
     */
    bool compileFromGLSL(const std::string& source,
                         ShaderStage stage = ShaderStage::Vertex,
                         const std::string& entryPoint = "main");
    
    /**
     * @brief Destroy the shader
     */
    ~Shader();
    
    // No copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // Moving
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    
    /**
     * @brief Check if shader is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan shader module
     */
    VkShaderModule getModule() const { return module_; }
    
    /**
     * @brief Get shader stage
     */
    ShaderStage getStage() const { return stage_; }
    
    /**
     * @brief Get entry point name
     */
    const std::string& getEntryPoint() const { return entryPoint_; }
    
    /**
     * @brief Get shader bytecode
     */
    const std::vector<uint32_t>& getCode() const { return code_; }
    
    /**
     * @brief Get reflection data
     */
    const ShaderReflection& getReflection() const { return reflection_; }
    
    /**
     * @brief Get reflection data (non-const)
     */
    ShaderReflection& getReflection() { return reflection_; }
    
    /**
     * @brief Set reflection data manually
     */
    void setReflection(const ShaderReflection& reflection);
    
    /**
     * @brief Check if shader has specific binding
     */
    bool hasBinding(uint32_t set, uint32_t binding) const;
    
    /**
     * @brief Get binding count
     */
    uint32_t getBindingCount() const { return reflection_.bindings.size(); }
    
    /**
     * @brief Create combined vertex + fragment shaders
     */
    static std::pair<Shader, Shader> createGraphicsPair(
        const std::string& vertexPath,
        const std::string& fragmentPath,
        const std::string& vertexEntry = "main",
        const std::string& fragmentEntry = "main");
    
private:
    bool createFromSPIRV(const void* spirv, size_t size,
                        ShaderStage stage, const std::string& entryPoint);
    void computeReflection();
    
    bool valid_ = false;
    VkShaderModule module_ = VK_NULL_HANDLE;
    ShaderStage stage_ = ShaderStage::Vertex;
    std::string entryPoint_;
    std::vector<uint32_t> code_;
    ShaderReflection reflection_;
};

/**
 * @brief Pre-compiled shader library
 * 
 * Manages a collection of shaders for easy access.
 * Typically used for built-in shaders (UI, primitives, etc.).
 */
class ShaderLibrary {
public:
    /**
     * @brief Get singleton instance
     */
    static ShaderLibrary& getInstance();
    
    /**
     * @brief Load a shader from file
     * @param name Identifier for the shader
     * @param path File path
     * @param stage Shader stage
     * @return Shared pointer to shader, or nullptr on failure
     */
    std::shared_ptr<Shader> load(const std::string& name,
                                  const std::string& path,
                                  ShaderStage stage = ShaderStage::Vertex);
    
    /**
     * @brief Get a loaded shader
     */
    std::shared_ptr<Shader> get(const std::string& name) const;
    
    /**
     * @brief Check if shader exists
     */
    bool has(const std::string& name) const;
    
    /**
     * @brief Unload a shader
     */
    void unload(const std::string& name);
    
    /**
     * @brief Clear all shaders
     */
    void clear();
    
    /**
     * @brief Get all shader names
     */
    std::vector<std::string> getShaderNames() const;
    
    /**
     * @brief Preload common shaders
     * @param basePath Base path for shader files
     */
    void preloadCommonShaders(const std::string& basePath);
    
private:
    ShaderLibrary() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
};

} // namespace vgui
