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

#include "vgui/graphics/shader.hpp"
#include "vgui/graphics/vertex.hpp"
#include "vgui/graphics/buffer.hpp"
#include "vgui/graphics/texture.hpp"
#include "vgui/graphics/descriptor.hpp"

namespace vgui {

/**
 * @brief Primitive topology
 */
enum class PrimitiveTopology {
    PointList = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    LineList = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    LineStrip = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    TriangleList = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    TriangleStrip = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
};

/**
 * @brief Polygon fill mode
 */
enum class PolygonMode {
    Fill = VK_POLYGON_MODE_FILL,
    Line = VK_POLYGON_MODE_LINE,
    Point = VK_POLYGON_MODE_POINT
};

/**
 * @brief Culling mode
 */
enum class CullMode {
    None = VK_CULL_MODE_NONE,
    Front = VK_CULL_MODE_FRONT_BIT,
    Back = VK_CULL_MODE_BACK_BIT,
    FrontAndBack = VK_CULL_MODE_FRONT_AND_BACK_BIT
};

/**
 * @brief Front face winding
 */
enum class FrontFace {
    CounterClockwise = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    Clockwise = VK_FRONT_FACE_CLOCKWISE
};

/**
 * @brief Depth test operation
 */
enum class CompareOp {
    Never = VK_COMPARE_OP_NEVER,
    Less = VK_COMPARE_OP_LESS,
    LessOrEqual = VK_COMPARE_OP_LESS_OR_EQUAL,
    Equal = VK_COMPARE_OP_EQUAL,
    GreaterOrEqual = VK_COMPARE_OP_GREATER_OR_EQUAL,
    Greater = VK_COMPARE_OP_GREATER,
    NotEqual = VK_COMPARE_OP_NOT_EQUAL,
    Always = VK_COMPARE_OP_ALWAYS
};

/**
 * @brief Stencil operation
 */
enum class StencilOp {
    Keep = VK_STENCIL_OP_KEEP,
    Zero = VK_STENCIL_OP_ZERO,
    Replace = VK_STENCIL_OP_REPLACE,
    IncrementAndClamp = VK_STENCIL_OP_INCREMENT_AND_CLAMP,
    DecrementAndClamp = VK_STENCIL_OP_DECREMENT_AND_CLAMP,
    Invert = VK_STENCIL_OP_INVERT,
    IncrementAndWrap = VK_STENCIL_OP_INCREMENT_AND_WRAP,
    DecrementAndWrap = VK_STENCIL_OP_DECREMENT_AND_WRAP
};

/**
 * @brief Color blend factor
 */
enum class BlendFactor {
    Zero = VK_BLEND_FACTOR_ZERO,
    One = VK_BLEND_FACTOR_ONE,
    SrcColor = VK_BLEND_FACTOR_SRC_COLOR,
    OneMinusSrcColor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    DstColor = VK_BLEND_FACTOR_DST_COLOR,
    OneMinusDstColor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    SrcAlpha = VK_BLEND_FACTOR_SRC_ALPHA,
    OneMinusSrcAlpha = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    DstAlpha = VK_BLEND_FACTOR_DST_ALPHA,
    OneMinusDstAlpha = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    ConstantColor = VK_BLEND_FACTOR_CONSTANT_COLOR,
    OneMinusConstantColor = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    ConstantAlpha = VK_BLEND_FACTOR_CONSTANT_ALPHA,
    OneMinusConstantAlpha = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
    SrcAlphaSaturate = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE
};

/**
 * @brief Blend operation
 */
enum class BlendOp {
    Add = VK_BLEND_OP_ADD,
    Subtract = VK_BLEND_OP_SUBTRACT,
    ReverseSubtract = VK_BLEND_OP_REVERSE_SUBTRACT,
    Min = VK_BLEND_OP_MIN,
    Max = VK_BLEND_OP_MAX
};

/**
 * @brief Pipeline color blend state
 */
struct ColorBlendAttachment {
    bool blendEnable = false;
    BlendFactor srcColorBlendFactor = BlendFactor::One;
    BlendFactor dstColorBlendFactor = BlendFactor::Zero;
    BlendOp colorBlendOp = BlendOp::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
    BlendOp alphaBlendOp = BlendOp::Add;
    uint32_t colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
};

/**
 * @brief Pipeline rasterization state
 */
struct RasterizationState {
    PolygonMode polygonMode = PolygonMode::Fill;
    CullMode cullMode = CullMode::Back;
    FrontFace frontFace = FrontFace::CounterClockwise;
    bool depthClampEnable = false;
    bool rasterizerDiscardEnable = false;
    bool depthBiasEnable = false;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasSlopeFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float lineWidth = 1.0f;
};

/**
 * @brief Pipeline depth stencil state
 */
struct DepthStencilState {
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    CompareOp depthCompareOp = CompareOp::Less;
    bool depthBoundsTestEnable = false;
    bool stencilTestEnable = false;
    uint32_t frontStencilFailOp = StencilOp::Keep;
    uint32_t frontStencilPassOp = StencilOp::Keep;
    uint32_t frontStencilDepthFailOp = StencilOp::Keep;
    uint32_t frontStencilCompareOp = CompareOp::Always;
    uint32_t backStencilFailOp = StencilOp::Keep;
    uint32_t backStencilPassOp = StencilOp::Keep;
    uint32_t backStencilDepthFailOp = StencilOp::Keep;
    uint32_t backStencilCompareOp = CompareOp::Always;
    float minDepthBounds = 0.0f;
    float maxDepthBounds = 1.0f;
};

/**
 * * @brief Pipeline multisample state
 */
struct MultisampleState {
    uint32_t rasterizationSamples = 1;
    bool sampleShadingEnable = false;
    float minSampleShading = 0.2f;
    uint32_t sampleMask = 0xFFFFFFFF;
    bool alphaToCoverageEnable = false;
    bool alphaToOneEnable = false;
};

/**
 * @brief Pipeline input assembly state
 */
struct InputAssemblyState {
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    bool primitiveRestartEnable = false;
    uint32_t stripVertexCeremony = 0;
};

/**
 * @brief Graphics pipeline configuration
 */
struct PipelineConfig {
    // Shaders
    std::shared_ptr<Shader> vertexShader;
    std::shared_ptr<Shader> fragmentShader;
    
    // Input assembly
    InputAssemblyState inputAssembly;
    
    // Viewports and scissors (dynamic)
    bool useDynamicViewport = true;
    bool useDynamicScissor = true;
    bool useDynamicLineWidth = false;
    
    // Rasterization
    RasterizationState rasterization;
    
    // Depth stencil
    std::shared_ptr<class DepthStencilStateObject> depthStencil;
    bool separateDepthStencil = false;
    
    // Multisample
    MultisampleState multisample;
    
    // Color blending
    std::vector<ColorBlendAttachment> colorBlendAttachments;
    bool logicOpEnable = false;
    uint32_t logicOp = VK_LOGIC_OP_COPY;
    
    // Render pass
    std::shared_ptr<class RenderPass> renderPass;
    uint32_t subpassIndex = 0;
    
    // Pipeline layout
    std::shared_ptr<class PipelineLayout> layout;
    
    // Vertex input
    std::vector<VertexElement> vertexAttributes;
    uint32_t vertexBindingStride = 0;
    uint32_t vertexBindingIndex = 0;
    
    // Specialization constants
    std::vector<class SpecializationConstant> specializations;
    
    // Pipeline derivatives
    bool basePipelineHandleEnable = false;
    VkPipeline basePipelineHandle = VK_NULL_HANDLE;
    int32_t basePipelineIndex = -1;
    
    // Debug name
    std::string debugName;
    
    /**
     * @brief Create default UI pipeline config
     */
    static PipelineConfig createUI();
    
    /**
     * @brief Create default solid mesh pipeline config
     */
    static PipelineConfig createSolidMesh();
    
    /**
     * @brief Create default textured mesh pipeline config
     */
    static PipelineConfig createTexturedMesh();
    
    /**
     * @brief Create default phong lit pipeline config
     */
    static PipelineConfig createPhongLit();
};

/**
 * @brief Graphics pipeline class
 * 
 * Encapsulates a Vulkan graphics pipeline with state management.
 */
class GraphicsPipeline {
public:
    /**
     * @brief Create an empty pipeline
     */
    GraphicsPipeline();
    
    /**
     * @brief Create pipeline from config
     */
    bool create(const PipelineConfig& config);
    
    /**
     * @brief Destroy the pipeline
     */
    ~GraphicsPipeline();
    
    // No copying
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    
    // Moving
    GraphicsPipeline(GraphicsPipeline&& other) noexcept;
    GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;
    
    /**
     * @brief Check if pipeline is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get Vulkan pipeline handle
     */
    VkPipeline getHandle() const { return pipeline_; }
    
    /**
     * @brief Get pipeline layout
     */
    VkPipelineLayout getLayout() const;
    
    /**
     * @brief Bind pipeline to command buffer
     */
    void bind(class CommandBuffer& cmdBuffer) const;
    
    /**
     * @brief Get shader
     */
    const Shader* getVertexShader() const { return config_.vertexShader.get(); }
    const Shader* getFragmentShader() const { return config_.fragmentShader.get(); }
    
    /**
     * @brief Get configuration
     */
    const PipelineConfig& getConfig() const { return config_; }
    
private:
    void destroy();
    
    bool valid_ = false;
    VkPipeline pipeline_ = VK_NULL_HANDLE;
    PipelineConfig config_;
};

/**
 * @brief Pipeline layout
 */
class PipelineLayout {
public:
    /**
     * @brief Create empty layout
     */
    PipelineLayout();
    
    /**
     * @brief Add descriptor set layout
     */
    void addDescriptorSetLayout(std::shared_ptr<class DescriptorSetLayout> layout);
    
    /**
     * @brief Set push constant ranges
     */
    void setPushConstantRanges(const std::vector<VkPushConstantRange>& ranges);
    
    /**
     * @brief Create the layout
     */
    bool create();
    
    /**
     * @brief Destroy
     */
    ~PipelineLayout();
    
    VkPipelineLayout getHandle() const { return handle_; }
    
private:
    std::vector<std::shared_ptr<class DescriptorSetLayout>> setLayouts_;
    std::vector<VkPushConstantRange> pushConstantRanges_;
    VkPipelineLayout handle_ = VK_NULL_HANDLE;
    bool valid_ = false;
};

/**
 * @brief Specialization constant
 */
struct SpecializationConstant {
    uint32_t constantID;
    VkSpecializationMapEntry mapEntry;
    std::vector<uint8_t> data;
    
    template<typename T>
    static SpecializationConstant fromValue(uint32_t id, T value) {
        SpecializationConstant sc;
        sc.constantID = id;
        sc.mapEntry = {id, 0, sizeof(T)};
        sc.data.assign(reinterpret_cast<const uint8_t*>(&value),
                      reinterpret_cast<const uint8_t*>(&value) + sizeof(T));
        return sc;
    }
};

} // namespace vgui
