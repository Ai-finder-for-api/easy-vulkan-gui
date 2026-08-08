#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Shader                                                 ║
// ║  Loads SPIR-V (Vulkan) or MSL/metallib (Metal).                ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace vgui {

enum class ShaderStage : uint8_t {
    Vertex,
    Fragment,
    Compute,
    Geometry,           ///< Vulkan only
    TessControl,        ///< Vulkan only
    TessEvaluation      ///< Vulkan only
};

class Shader {
public:
    /// Load from pre-compiled SPIR-V or .metallib file
    static std::shared_ptr<Shader> load_file(const std::string& path,
                                              ShaderStage        stage,
                                              const std::string& entry = "main");

    /// Load from in-memory SPIR-V bytes
    static std::shared_ptr<Shader> load_spirv(const std::vector<uint32_t>& spirv,
                                               ShaderStage                  stage,
                                               const std::string&           entry = "main");

    /// Load from MSL source string (Metal only; compiles at runtime)
    static std::shared_ptr<Shader> load_msl(const std::string& source,
                                             ShaderStage        stage,
                                             const std::string& entry = "main");

    ShaderStage stage()  const;
    const std::string& entry_point() const;
    void* gpu_handle()   const;   ///< VkShaderModule / id<MTLFunction>

private:
    Shader() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
