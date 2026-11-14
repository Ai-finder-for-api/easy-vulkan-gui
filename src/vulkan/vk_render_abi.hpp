// Vulkan 1.0 layouts used by the render path (graphics pipeline, render pass, images, buffers, memory).
// Hand-written like the rest of src/vulkan: every struct matches the spec's field order and 64-bit sizes.
// Reuses vkp:: types from vk_types.hpp (Range, ImageMemoryBarrier, SubmitInfo, Cmd, ...). Internal to src/vulkan.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include "vk_types.hpp"

namespace evgx { namespace vkr {

using namespace vkp;                                     // R, H, Dev, Queue, Cmd, Range, ImageMemoryBarrier, ...

struct Offset2D { int32_t x, y; };
struct Extent2D { uint32_t width, height; };
struct Offset3D { int32_t x, y, z; };
struct Rect2D { Offset2D offset; Extent2D extent; };
struct Viewport { float x, y, width, height, minDepth, maxDepth; };
struct SubresourceLayers { uint32_t aspectMask, mipLevel, baseArrayLayer, layerCount; };
struct ComponentMapping { uint32_t r, g, b, a; };
struct ClearValue { uint32_t raw[4]; };                  // color (floats) or depth-stencil; 16 bytes either way

struct ImageCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, imageType, format;
    Ext3D extent; uint32_t mipLevels, arrayLayers, samples, tiling, usage, sharingMode;
    uint32_t queueFamilyIndexCount; const uint32_t* pQueueFamilyIndices; uint32_t initialLayout;
};
struct ImageViewCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags; H image; uint32_t viewType, format;
    ComponentMapping components; Range subresourceRange;
};
struct BufferCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags; uint64_t size; uint32_t usage, sharingMode;
    uint32_t queueFamilyIndexCount; const uint32_t* pQueueFamilyIndices;
};
struct MemoryRequirements { uint64_t size, alignment; uint32_t memoryTypeBits; };
struct MemoryAllocateInfo { uint32_t sType; const void* pNext; uint64_t allocationSize; uint32_t memoryTypeIndex; };
struct MemoryType { uint32_t propertyFlags, heapIndex; };
struct MemoryHeap { uint64_t size; uint32_t flags; };
struct PhysicalDeviceMemoryProperties { uint32_t memoryTypeCount; MemoryType memoryTypes[32]; uint32_t memoryHeapCount; MemoryHeap memoryHeaps[16]; };
struct BufferImageCopy { uint64_t bufferOffset; uint32_t bufferRowLength, bufferImageHeight; SubresourceLayers imageSubresource; Offset3D imageOffset; Ext3D imageExtent; };
struct ImageCopy { SubresourceLayers srcSubresource; Offset3D srcOffset; SubresourceLayers dstSubresource; Offset3D dstOffset; Ext3D extent; };
struct BufferCopy { uint64_t srcOffset, dstOffset, size; };

struct ShaderModuleCreateInfo { uint32_t sType; const void* pNext; uint32_t flags; size_t codeSize; const uint32_t* pCode; };
struct PipelineShaderStageCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, stage; H module; const char* pName; const void* pSpecializationInfo; };
struct VertexInputBindingDescription { uint32_t binding, stride, inputRate; };
struct VertexInputAttributeDescription { uint32_t location, binding, format, offset; };
struct PipelineVertexInputStateCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags;
    uint32_t vertexBindingDescriptionCount; const VertexInputBindingDescription* pVertexBindingDescriptions;
    uint32_t vertexAttributeDescriptionCount; const VertexInputAttributeDescription* pVertexAttributeDescriptions;
};
struct PipelineInputAssemblyStateCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, topology, primitiveRestartEnable; };
struct PipelineViewportStateCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, viewportCount; const Viewport* pViewports; uint32_t scissorCount; const Rect2D* pScissors;
};
struct PipelineRasterizationStateCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, depthClampEnable, rasterizerDiscardEnable, polygonMode, cullMode, frontFace, depthBiasEnable;
    float depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor, lineWidth;
};
struct PipelineMultisampleStateCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, rasterizationSamples, sampleShadingEnable; float minSampleShading;
    const uint32_t* pSampleMask; uint32_t alphaToCoverageEnable, alphaToOneEnable;
};
struct StencilOpState { uint32_t failOp, passOp, depthFailOp, compareOp, compareMask, writeMask, reference; };
struct PipelineDepthStencilStateCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, depthTestEnable, depthWriteEnable, depthCompareOp, depthBoundsTestEnable, stencilTestEnable;
    StencilOpState front, back; float minDepthBounds, maxDepthBounds;
};
struct PipelineColorBlendAttachmentState {
    uint32_t blendEnable, srcColorBlendFactor, dstColorBlendFactor, colorBlendOp, srcAlphaBlendFactor, dstAlphaBlendFactor, alphaBlendOp, colorWriteMask;
};
struct PipelineColorBlendStateCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, logicOpEnable, logicOp, attachmentCount;
    const PipelineColorBlendAttachmentState* pAttachments; float blendConstants[4];
};
struct PipelineDynamicStateCreateInfo { uint32_t sType; const void* pNext; uint32_t flags, dynamicStateCount; const uint32_t* pDynamicStates; };
struct PushConstantRange { uint32_t stageFlags, offset, size; };
struct PipelineLayoutCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, setLayoutCount; const H* pSetLayouts; uint32_t pushConstantRangeCount; const PushConstantRange* pPushConstantRanges;
};
struct AttachmentDescription { uint32_t flags, format, samples, loadOp, storeOp, stencilLoadOp, stencilStoreOp, initialLayout, finalLayout; };
struct AttachmentReference { uint32_t attachment, layout; };
struct SubpassDescription {
    uint32_t flags, pipelineBindPoint; uint32_t inputAttachmentCount; const AttachmentReference* pInputAttachments;
    uint32_t colorAttachmentCount; const AttachmentReference* pColorAttachments; const AttachmentReference* pResolveAttachments;
    const AttachmentReference* pDepthStencilAttachment; uint32_t preserveAttachmentCount; const uint32_t* pPreserveAttachments;
};
struct SubpassDependency { uint32_t srcSubpass, dstSubpass, srcStageMask, dstStageMask, srcAccessMask, dstAccessMask, dependencyFlags; };
struct RenderPassCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags; uint32_t attachmentCount; const AttachmentDescription* pAttachments;
    uint32_t subpassCount; const SubpassDescription* pSubpasses; uint32_t dependencyCount; const SubpassDependency* pDependencies;
};
struct FramebufferCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags; H renderPass; uint32_t attachmentCount; const H* pAttachments;
    uint32_t width, height, layers;
};
struct RenderPassBeginInfo {
    uint32_t sType; const void* pNext; H renderPass; H framebuffer; Rect2D renderArea; uint32_t clearValueCount; const ClearValue* pClearValues;
};
struct GraphicsPipelineCreateInfo {
    uint32_t sType; const void* pNext; uint32_t flags, stageCount; const PipelineShaderStageCreateInfo* pStages;
    const PipelineVertexInputStateCreateInfo* pVertexInputState; const PipelineInputAssemblyStateCreateInfo* pInputAssemblyState;
    const void* pTessellationState; const PipelineViewportStateCreateInfo* pViewportState;
    const PipelineRasterizationStateCreateInfo* pRasterizationState; const PipelineMultisampleStateCreateInfo* pMultisampleState;
    const PipelineDepthStencilStateCreateInfo* pDepthStencilState; const PipelineColorBlendStateCreateInfo* pColorBlendState;
    const PipelineDynamicStateCreateInfo* pDynamicState; H layout; H renderPass; uint32_t subpass; H basePipelineHandle; int32_t basePipelineIndex;
};

// structure types, enum values and flags (Vulkan 1.0)
enum : uint32_t {
    S_IMAGE_VIEW = 15, S_SHADER_MODULE = 16, S_PIPELINE_SHADER_STAGE = 18, S_PIPELINE_VERTEX_INPUT = 19, S_PIPELINE_INPUT_ASSEMBLY = 20,
    S_PIPELINE_VIEWPORT = 22, S_PIPELINE_RASTERIZATION = 23, S_PIPELINE_MULTISAMPLE = 24, S_PIPELINE_DEPTH_STENCIL = 25,
    S_PIPELINE_COLOR_BLEND = 26, S_PIPELINE_DYNAMIC = 27, S_GRAPHICS_PIPELINE = 28, S_PIPELINE_LAYOUT = 30, S_FRAMEBUFFER = 37,
    S_RENDER_PASS = 38, S_RENDER_PASS_BEGIN = 43, S_MEMORY_ALLOCATE_INFO = 5, S_BUFFER_CREATE_INFO = 12, S_IMAGE_CREATE_INFO = 14,
    FORMAT_R32G32B32_SFLOAT = 106, FORMAT_D32_SFLOAT = 126, FORMAT_B8G8R8A8_UNORM = 44,
    LAYOUT_COLOR_ATTACHMENT = 2, LAYOUT_DEPTH_ATTACHMENT = 3, LAYOUT_TRANSFER_SRC = 6,
    IMAGE_USAGE_TRANSFER_SRC = 0x1, IMAGE_USAGE_TRANSFER_DST = 0x2, IMAGE_USAGE_COLOR_ATTACHMENT = 0x10, IMAGE_USAGE_DEPTH_ATTACHMENT = 0x20,
    ASPECT_DEPTH = 0x2, VIEW_TYPE_2D = 1,
    LOAD_CLEAR = 1, LOAD_DONT_CARE = 2, STORE_STORE = 0, STORE_DONT_CARE = 1,
    STAGE_VERTEX_INPUT = 0x4, STAGE_VERTEX_SHADER = 0x8, STAGE_EARLY_FRAGMENT = 0x100, STAGE_LATE_FRAGMENT = 0x200,
    STAGE_COLOR_OUTPUT = 0x400, STAGE_ALL_GRAPHICS = 0x8000,
    ACCESS_INDEX_READ = 0x2, ACCESS_VERTEX_READ = 0x4, ACCESS_COLOR_READ = 0x80, ACCESS_COLOR_WRITE = 0x100,
    ACCESS_DEPTH_WRITE = 0x400, ACCESS_TRANSFER_READ = 0x800,
    STAGE_SHADER_VERTEX = 0x1, STAGE_SHADER_FRAGMENT = 0x10,
    TOPOLOGY_TRIANGLE_LIST = 3, POLYGON_FILL = 0, CULL_NONE = 0, FRONT_CCW = 0,
    COMPARE_LESS = 1, COLOR_WRITE_ALL = 0xF, DYNAMIC_VIEWPORT = 0, DYNAMIC_SCISSOR = 1,
    INDEX_UINT32 = 1, BIND_GRAPHICS = 0, SUBPASS_CONTENTS_INLINE = 0,
    SUBPASS_EXTERNAL = 0xFFFFFFFFu, PIPELINE_BIND_GRAPHICS = 0,
    MEM_DEVICE_LOCAL = 0x1, MEM_HOST_VISIBLE = 0x2, MEM_HOST_COHERENT = 0x4,
    BUF_USAGE_VERTEX = 0x80, BUF_USAGE_INDEX = 0x40, BUF_USAGE_TRANSFER_DST = 0x2, BUF_USAGE_TRANSFER_SRC = 0x1,
    CMD_ONE_TIME_SUBMIT = 0x1,
};

// function pointers for the render path, loaded from the device once
typedef R (*PFN_vkCreateImageR)(Dev, const ImageCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyImageR)(Dev, H, const void*);
typedef void (*PFN_vkGetImageMemoryRequirementsR)(Dev, H, MemoryRequirements*);
typedef R (*PFN_vkBindImageMemoryR)(Dev, H, H, uint64_t);
typedef R (*PFN_vkCreateImageViewR)(Dev, const ImageViewCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyImageViewR)(Dev, H, const void*);
typedef R (*PFN_vkCreateBufferR)(Dev, const BufferCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyBufferR)(Dev, H, const void*);
typedef void (*PFN_vkGetBufferMemoryRequirementsR)(Dev, H, MemoryRequirements*);
typedef R (*PFN_vkBindBufferMemoryR)(Dev, H, H, uint64_t);
typedef R (*PFN_vkAllocateMemoryR)(Dev, const MemoryAllocateInfo*, const void*, H*);
typedef void (*PFN_vkFreeMemoryR)(Dev, H, const void*);
typedef R (*PFN_vkMapMemoryR)(Dev, H, uint64_t, uint64_t, uint32_t, void**);
typedef void (*PFN_vkUnmapMemoryR)(Dev, H);
typedef R (*PFN_vkCreateShaderModuleR)(Dev, const ShaderModuleCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyShaderModuleR)(Dev, H, const void*);
typedef R (*PFN_vkCreatePipelineLayoutR)(Dev, const PipelineLayoutCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyPipelineLayoutR)(Dev, H, const void*);
typedef R (*PFN_vkCreateRenderPassR)(Dev, const RenderPassCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyRenderPassR)(Dev, H, const void*);
typedef R (*PFN_vkCreateFramebufferR)(Dev, const FramebufferCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyFramebufferR)(Dev, H, const void*);
typedef R (*PFN_vkCreateGraphicsPipelinesR)(Dev, H, uint32_t, const GraphicsPipelineCreateInfo*, const void*, H*);
typedef void (*PFN_vkDestroyPipelineR)(Dev, H, const void*);
typedef void (*PFN_vkCmdBeginRenderPassR)(Cmd, const RenderPassBeginInfo*, uint32_t);
typedef void (*PFN_vkCmdEndRenderPassR)(Cmd);
typedef void (*PFN_vkCmdBindPipelineR)(Cmd, uint32_t, H);
typedef void (*PFN_vkCmdBindVertexBuffersR)(Cmd, uint32_t, uint32_t, const H*, const uint64_t*);
typedef void (*PFN_vkCmdBindIndexBufferR)(Cmd, H, uint64_t, uint32_t);
typedef void (*PFN_vkCmdPushConstantsR)(Cmd, H, uint32_t, uint32_t, uint32_t, const void*);
typedef void (*PFN_vkCmdDrawIndexedR)(Cmd, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
typedef void (*PFN_vkCmdSetViewportR)(Cmd, uint32_t, uint32_t, const Viewport*);
typedef void (*PFN_vkCmdSetScissorR)(Cmd, uint32_t, uint32_t, const Rect2D*);
typedef void (*PFN_vkCmdCopyImageR)(Cmd, H, uint32_t, H, uint32_t, uint32_t, const ImageCopy*);
typedef void (*PFN_vkCmdCopyImageToBufferR)(Cmd, H, uint32_t, H, uint32_t, const BufferImageCopy*);
typedef void (*PFN_vkCmdCopyBufferR)(Cmd, H, H, uint32_t, const BufferCopy*);
typedef R (*PFN_vkQueueWaitIdleR)(Queue);
typedef void (*PFN_vkGetPhysicalDeviceMemoryPropertiesR)(PDev, PhysicalDeviceMemoryProperties*);

}}  // namespace evgx::vkr
