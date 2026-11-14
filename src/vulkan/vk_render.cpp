// Mesh rendering on the GPU: render pass, graphics pipeline, per-size render targets, per-model vertex and index
// buffers, and the two paths that use them (into a window's swapchain, or into pixels read back to the CPU).
// Everything runs on the shared context from vk_device.cpp. Each submit is followed by a queue wait, so no GPU object
// is still in use when it is replaced or destroyed; that keeps the code simple at some cost in speed.
#include "vk_render.hpp"
#include "vk_render_abi.hpp"
#include "mesh_spv.hpp"
#include <evg/scene.hpp>
#include "../core/mem.hpp"

namespace evgx { namespace vkp {

namespace {

using namespace vkr;

struct Vertex { float p[3], n[3], c[3]; };          // 36 bytes, matches the attribute layout in the pipeline

struct RenderFns {
    PFN_vkCreateImageR createImage; PFN_vkDestroyImageR destroyImage;
    PFN_vkGetImageMemoryRequirementsR imageMemReq; PFN_vkBindImageMemoryR bindImageMemory;
    PFN_vkCreateImageViewR createImageView; PFN_vkDestroyImageViewR destroyImageView;
    PFN_vkCreateBufferR createBuffer; PFN_vkDestroyBufferR destroyBuffer;
    PFN_vkGetBufferMemoryRequirementsR bufferMemReq; PFN_vkBindBufferMemoryR bindBufferMemory;
    PFN_vkAllocateMemoryR allocMemory; PFN_vkFreeMemoryR freeMemory;
    PFN_vkMapMemoryR mapMemory; PFN_vkUnmapMemoryR unmapMemory;
    PFN_vkCreateShaderModuleR createShaderModule; PFN_vkDestroyShaderModuleR destroyShaderModule;
    PFN_vkCreatePipelineLayoutR createPipelineLayout; PFN_vkDestroyPipelineLayoutR destroyPipelineLayout;
    PFN_vkCreateRenderPassR createRenderPass; PFN_vkDestroyRenderPassR destroyRenderPass;
    PFN_vkCreateFramebufferR createFramebuffer; PFN_vkDestroyFramebufferR destroyFramebuffer;
    PFN_vkCreateGraphicsPipelinesR createPipelines; PFN_vkDestroyPipelineR destroyPipeline;
    PFN_vkCmdBeginRenderPassR cmdBeginRenderPass; PFN_vkCmdEndRenderPassR cmdEndRenderPass;
    PFN_vkCmdBindPipelineR cmdBindPipeline; PFN_vkCmdBindVertexBuffersR cmdBindVertexBuffers;
    PFN_vkCmdBindIndexBufferR cmdBindIndexBuffer; PFN_vkCmdPushConstantsR cmdPushConstants;
    PFN_vkCmdDrawIndexedR cmdDrawIndexed; PFN_vkCmdSetViewportR cmdSetViewport; PFN_vkCmdSetScissorR cmdSetScissor;
    PFN_vkCmdCopyImageR cmdCopyImage; PFN_vkCmdCopyImageToBufferR cmdCopyImageToBuffer;
    PFN_vkQueueWaitIdleR queueWaitIdle;
    PFN_vkGetPhysicalDeviceMemoryPropertiesR getMemoryProperties;
};

struct Target {
    uint32_t w = 0, h = 0;
    H color = 0, colorMem = 0, colorView = 0;
    H depth = 0, depthMem = 0, depthView = 0;
    H fb = 0;
    H rbuf = 0, rbufMem = 0;                          // host-visible copy of the color image, for readback
};

struct GpuMesh {
    const Model* key = nullptr;
    H vb = 0, vbMem = 0, ib = 0, ibMem = 0;
    uint32_t* firstIndex = nullptr;                   // per primitive: first index in the index buffer
    uint32_t* indexCount = nullptr;                   // per primitive: 0 when the primitive is not drawable
    int32_t* vertexOffset = nullptr;                  // per primitive: added to every index of the primitive
    int nprims = 0;
};

constexpr int kMaxMeshes = 64;

struct State {
    bool ready = false;
    RenderFns f;
    PhysicalDeviceMemoryProperties mp;
    H vs = 0, fs = 0, layout = 0, renderPass = 0, pipeline = 0;
    Cmd cmd = nullptr;
    H fence = 0;
    Target target;
    GpuMesh meshes[kMaxMeshes];
    int nmeshes = 0;
};
State S;

// ---- small helpers ----

// bit pattern of a float, for ClearValue (avoids a union in the public struct layout)
uint32_t bits_of(float v) { union { float f; uint32_t u; } x; x.f = v; return x.u; }

bool fail(const char** err, const char* msg) { *err = msg; return false; }

// index of the first memory type allowed by `typeBits` that has all `flags`; -1 when none
int find_memtype(uint32_t typeBits, uint32_t flags) {
    for (uint32_t i = 0; i < S.mp.memoryTypeCount; i++)
        if ((typeBits & (1u << i)) && (S.mp.memoryTypes[i].propertyFlags & flags) == flags) return (int)i;
    return -1;
}

// allocates memory for an image or buffer and binds it
bool bind_memory(H res, bool isImage, uint32_t flags, H& mem, const char** err) {
    MemoryRequirements mr = {};
    if (isImage) S.f.imageMemReq(g_ctx.dev, res, &mr);
    else S.f.bufferMemReq(g_ctx.dev, res, &mr);
    int t = find_memtype(mr.memoryTypeBits, flags);
    if (t < 0) return fail(err, "no suitable GPU memory type");
    MemoryAllocateInfo mai = {};
    mai.sType = S_MEMORY_ALLOCATE_INFO;
    mai.allocationSize = mr.size;
    mai.memoryTypeIndex = (uint32_t)t;
    if (S.f.allocMemory(g_ctx.dev, &mai, nullptr, &mem) != 0) return fail(err, "vkAllocateMemory failed");
    if (isImage) S.f.bindImageMemory(g_ctx.dev, res, mem, 0);
    else S.f.bindBufferMemory(g_ctx.dev, res, mem, 0);
    return true;
}

bool make_image(uint32_t w, uint32_t h, uint32_t format, uint32_t usage, uint32_t aspect, H& image, H& mem, H& view, const char** err) {
    ImageCreateInfo ici = {};
    ici.sType = S_IMAGE_CREATE_INFO;
    ici.imageType = 1;                                // 2D
    ici.format = format;
    ici.extent = Ext3D{w, h, 1};
    ici.mipLevels = 1;
    ici.arrayLayers = 1;
    ici.samples = 1;
    ici.usage = usage;
    ici.initialLayout = LAYOUT_UNDEFINED;
    if (S.f.createImage(g_ctx.dev, &ici, nullptr, &image) != 0) return fail(err, "vkCreateImage failed");
    if (!bind_memory(image, true, MEM_DEVICE_LOCAL, mem, err)) return false;
    ImageViewCreateInfo vci = {};
    vci.sType = S_IMAGE_VIEW;
    vci.image = image;
    vci.viewType = VIEW_TYPE_2D;
    vci.format = format;
    vci.subresourceRange = Range{aspect, 0, 1, 0, 1};
    if (S.f.createImageView(g_ctx.dev, &vci, nullptr, &view) != 0) return fail(err, "vkCreateImageView failed");
    return true;
}

bool make_buffer(uint64_t bytes, uint32_t usage, H& buf, H& mem, const char** err) {
    BufferCreateInfo bci = {};
    bci.sType = S_BUFFER_CREATE_INFO;
    bci.size = bytes;
    bci.usage = usage;
    if (S.f.createBuffer(g_ctx.dev, &bci, nullptr, &buf) != 0) return fail(err, "vkCreateBuffer failed");
    return bind_memory(buf, false, MEM_HOST_VISIBLE | MEM_HOST_COHERENT, mem, err);
}

// copies `bytes` into a host-visible buffer
bool upload(H mem, const void* src, uint64_t bytes, const char** err) {
    void* p = nullptr;
    if (S.f.mapMemory(g_ctx.dev, mem, 0, bytes, 0, &p) != 0 || !p) return fail(err, "vkMapMemory failed");
    const uint8_t* s = (const uint8_t*)src;
    uint8_t* d = (uint8_t*)p;
    for (uint64_t i = 0; i < bytes; i++) d[i] = s[i];
    S.f.unmapMemory(g_ctx.dev, mem);
    return true;
}

bool download(H mem, void* dst, uint64_t bytes, const char** err) {
    void* p = nullptr;
    if (S.f.mapMemory(g_ctx.dev, mem, 0, bytes, 0, &p) != 0 || !p) return fail(err, "vkMapMemory failed");
    const uint8_t* s = (const uint8_t*)p;
    uint8_t* d = (uint8_t*)dst;
    for (uint64_t i = 0; i < bytes; i++) d[i] = s[i];
    S.f.unmapMemory(g_ctx.dev, mem);
    return true;
}

// ---- loading the render entry points and creating the fixed objects (once per device) ----

bool load_functions(const char** err) {
    auto get = [](const char* name) { return g_ctx.getDevProc(g_ctx.dev, name); };
    RenderFns& f = S.f;
    f.createImage = (PFN_vkCreateImageR)get("vkCreateImage");
    f.destroyImage = (PFN_vkDestroyImageR)get("vkDestroyImage");
    f.imageMemReq = (PFN_vkGetImageMemoryRequirementsR)get("vkGetImageMemoryRequirements");
    f.bindImageMemory = (PFN_vkBindImageMemoryR)get("vkBindImageMemory");
    f.createImageView = (PFN_vkCreateImageViewR)get("vkCreateImageView");
    f.destroyImageView = (PFN_vkDestroyImageViewR)get("vkDestroyImageView");
    f.createBuffer = (PFN_vkCreateBufferR)get("vkCreateBuffer");
    f.destroyBuffer = (PFN_vkDestroyBufferR)get("vkDestroyBuffer");
    f.bufferMemReq = (PFN_vkGetBufferMemoryRequirementsR)get("vkGetBufferMemoryRequirements");
    f.bindBufferMemory = (PFN_vkBindBufferMemoryR)get("vkBindBufferMemory");
    f.allocMemory = (PFN_vkAllocateMemoryR)get("vkAllocateMemory");
    f.freeMemory = (PFN_vkFreeMemoryR)get("vkFreeMemory");
    f.mapMemory = (PFN_vkMapMemoryR)get("vkMapMemory");
    f.unmapMemory = (PFN_vkUnmapMemoryR)get("vkUnmapMemory");
    f.createShaderModule = (PFN_vkCreateShaderModuleR)get("vkCreateShaderModule");
    f.destroyShaderModule = (PFN_vkDestroyShaderModuleR)get("vkDestroyShaderModule");
    f.createPipelineLayout = (PFN_vkCreatePipelineLayoutR)get("vkCreatePipelineLayout");
    f.destroyPipelineLayout = (PFN_vkDestroyPipelineLayoutR)get("vkDestroyPipelineLayout");
    f.createRenderPass = (PFN_vkCreateRenderPassR)get("vkCreateRenderPass");
    f.destroyRenderPass = (PFN_vkDestroyRenderPassR)get("vkDestroyRenderPass");
    f.createFramebuffer = (PFN_vkCreateFramebufferR)get("vkCreateFramebuffer");
    f.destroyFramebuffer = (PFN_vkDestroyFramebufferR)get("vkDestroyFramebuffer");
    f.createPipelines = (PFN_vkCreateGraphicsPipelinesR)get("vkCreateGraphicsPipelines");
    f.destroyPipeline = (PFN_vkDestroyPipelineR)get("vkDestroyPipeline");
    f.cmdBeginRenderPass = (PFN_vkCmdBeginRenderPassR)get("vkCmdBeginRenderPass");
    f.cmdEndRenderPass = (PFN_vkCmdEndRenderPassR)get("vkCmdEndRenderPass");
    f.cmdBindPipeline = (PFN_vkCmdBindPipelineR)get("vkCmdBindPipeline");
    f.cmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffersR)get("vkCmdBindVertexBuffers");
    f.cmdBindIndexBuffer = (PFN_vkCmdBindIndexBufferR)get("vkCmdBindIndexBuffer");
    f.cmdPushConstants = (PFN_vkCmdPushConstantsR)get("vkCmdPushConstants");
    f.cmdDrawIndexed = (PFN_vkCmdDrawIndexedR)get("vkCmdDrawIndexed");
    f.cmdSetViewport = (PFN_vkCmdSetViewportR)get("vkCmdSetViewport");
    f.cmdSetScissor = (PFN_vkCmdSetScissorR)get("vkCmdSetScissor");
    f.cmdCopyImage = (PFN_vkCmdCopyImageR)get("vkCmdCopyImage");
    f.cmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBufferR)get("vkCmdCopyImageToBuffer");
    f.queueWaitIdle = (PFN_vkQueueWaitIdleR)get("vkQueueWaitIdle");
    f.getMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryPropertiesR)g_ctx.gipa(g_ctx.inst, "vkGetPhysicalDeviceMemoryProperties");
    if (!f.createImage || !f.createImageView || !f.createBuffer || !f.allocMemory || !f.mapMemory || !f.createPipelines ||
        !f.createRenderPass || !f.createFramebuffer || !f.cmdBeginRenderPass || !f.cmdDrawIndexed || !f.cmdCopyImage ||
        !f.cmdCopyImageToBuffer || !f.queueWaitIdle || !f.getMemoryProperties)
        return fail(err, "a Vulkan render function is missing from the driver");
    f.getMemoryProperties(g_ctx.pd, &S.mp);
    return true;
}

bool make_shader(const uint32_t* code, uint32_t words, H& module, const char** err) {
    ShaderModuleCreateInfo smci = {};
    smci.sType = S_SHADER_MODULE;
    smci.codeSize = (size_t)words * 4;
    smci.pCode = code;
    if (S.f.createShaderModule(g_ctx.dev, &smci, nullptr, &module) != 0) return fail(err, "vkCreateShaderModule failed");
    return true;
}

bool create_pipeline(const char** err) {
    if (!make_shader(k_mesh_vert, k_mesh_vert_words, S.vs, err)) return false;
    if (!make_shader(k_mesh_frag, k_mesh_frag_words, S.fs, err)) return false;

    // push constants: view-projection and model matrices, visible to both stages
    PushConstantRange pcr = {STAGE_SHADER_VERTEX | STAGE_SHADER_FRAGMENT, 0, 128};
    PipelineLayoutCreateInfo plci = {};
    plci.sType = S_PIPELINE_LAYOUT;
    plci.pushConstantRangeCount = 1;
    plci.pPushConstantRanges = &pcr;
    if (S.f.createPipelineLayout(g_ctx.dev, &plci, nullptr, &S.layout) != 0) return fail(err, "vkCreatePipelineLayout failed");

    // render pass: color (ends in TRANSFER_SRC so it can be copied) and depth
    AttachmentDescription att[2] = {};
    att[0].format = FORMAT_B8G8R8A8_UNORM; att[0].samples = 1;
    att[0].loadOp = LOAD_CLEAR; att[0].storeOp = STORE_STORE;
    att[0].stencilLoadOp = LOAD_DONT_CARE; att[0].stencilStoreOp = STORE_DONT_CARE;
    att[0].initialLayout = LAYOUT_UNDEFINED; att[0].finalLayout = LAYOUT_TRANSFER_SRC;
    att[1].format = FORMAT_D32_SFLOAT; att[1].samples = 1;
    att[1].loadOp = LOAD_CLEAR; att[1].storeOp = STORE_DONT_CARE;
    att[1].stencilLoadOp = LOAD_DONT_CARE; att[1].stencilStoreOp = STORE_DONT_CARE;
    att[1].initialLayout = LAYOUT_UNDEFINED; att[1].finalLayout = LAYOUT_DEPTH_ATTACHMENT;
    AttachmentReference colorRef = {0, LAYOUT_COLOR_ATTACHMENT};
    AttachmentReference depthRef = {1, LAYOUT_DEPTH_ATTACHMENT};
    SubpassDescription sub = {};
    sub.pipelineBindPoint = PIPELINE_BIND_GRAPHICS;
    sub.colorAttachmentCount = 1; sub.pColorAttachments = &colorRef;
    sub.pDepthStencilAttachment = &depthRef;
    SubpassDependency dep[2] = {};
    dep[0].srcSubpass = SUBPASS_EXTERNAL; dep[0].dstSubpass = 0;          // wait for the previous use of the attachments
    dep[0].srcStageMask = STAGE_COLOR_OUTPUT | STAGE_EARLY_FRAGMENT; dep[0].dstStageMask = STAGE_COLOR_OUTPUT | STAGE_EARLY_FRAGMENT;
    dep[0].dstAccessMask = ACCESS_COLOR_WRITE | ACCESS_DEPTH_WRITE;
    dep[1].srcSubpass = 0; dep[1].dstSubpass = SUBPASS_EXTERNAL;          // the copy after the pass reads the color
    dep[1].srcStageMask = STAGE_COLOR_OUTPUT; dep[1].dstStageMask = vkp::STAGE_TRANSFER;
    dep[1].srcAccessMask = ACCESS_COLOR_WRITE; dep[1].dstAccessMask = ACCESS_TRANSFER_READ;
    RenderPassCreateInfo rpci = {};
    rpci.sType = S_RENDER_PASS;
    rpci.attachmentCount = 2; rpci.pAttachments = att;
    rpci.subpassCount = 1; rpci.pSubpasses = &sub;
    rpci.dependencyCount = 2; rpci.pDependencies = dep;
    if (S.f.createRenderPass(g_ctx.dev, &rpci, nullptr, &S.renderPass) != 0) return fail(err, "vkCreateRenderPass failed");

    // vertex layout: position, normal, color (three vec3 of 12 bytes each)
    VertexInputBindingDescription bind = {0, sizeof(Vertex), 0};
    VertexInputAttributeDescription attrs[3] = {
        {0, 0, FORMAT_R32G32B32_SFLOAT, 0},
        {1, 0, FORMAT_R32G32B32_SFLOAT, 12},
        {2, 0, FORMAT_R32G32B32_SFLOAT, 24},
    };
    PipelineShaderStageCreateInfo stages[2] = {};
    stages[0].sType = S_PIPELINE_SHADER_STAGE; stages[0].stage = STAGE_SHADER_VERTEX; stages[0].module = S.vs; stages[0].pName = "main";
    stages[1].sType = S_PIPELINE_SHADER_STAGE; stages[1].stage = STAGE_SHADER_FRAGMENT; stages[1].module = S.fs; stages[1].pName = "main";
    PipelineVertexInputStateCreateInfo vis = {};
    vis.sType = S_PIPELINE_VERTEX_INPUT;
    vis.vertexBindingDescriptionCount = 1; vis.pVertexBindingDescriptions = &bind;
    vis.vertexAttributeDescriptionCount = 3; vis.pVertexAttributeDescriptions = attrs;
    PipelineInputAssemblyStateCreateInfo ias = {};
    ias.sType = S_PIPELINE_INPUT_ASSEMBLY; ias.topology = TOPOLOGY_TRIANGLE_LIST;
    PipelineViewportStateCreateInfo vps = {};                          // viewport and scissor are set per draw
    vps.sType = S_PIPELINE_VIEWPORT; vps.viewportCount = 1; vps.scissorCount = 1;
    PipelineRasterizationStateCreateInfo rs = {};
    rs.sType = S_PIPELINE_RASTERIZATION; rs.polygonMode = POLYGON_FILL; rs.cullMode = CULL_NONE; rs.frontFace = FRONT_CCW; rs.lineWidth = 1.0f;
    PipelineMultisampleStateCreateInfo ms = {};
    ms.sType = S_PIPELINE_MULTISAMPLE; ms.rasterizationSamples = 1;
    PipelineDepthStencilStateCreateInfo ds = {};
    ds.sType = S_PIPELINE_DEPTH_STENCIL; ds.depthTestEnable = 1; ds.depthWriteEnable = 1; ds.depthCompareOp = COMPARE_LESS;
    PipelineColorBlendAttachmentState cba = {};
    cba.colorWriteMask = COLOR_WRITE_ALL;
    PipelineColorBlendStateCreateInfo cbs = {};
    cbs.sType = S_PIPELINE_COLOR_BLEND; cbs.attachmentCount = 1; cbs.pAttachments = &cba;
    uint32_t dyn[2] = {DYNAMIC_VIEWPORT, DYNAMIC_SCISSOR};
    PipelineDynamicStateCreateInfo dsi = {};
    dsi.sType = S_PIPELINE_DYNAMIC; dsi.dynamicStateCount = 2; dsi.pDynamicStates = dyn;
    GraphicsPipelineCreateInfo gpci = {};
    gpci.sType = S_GRAPHICS_PIPELINE;
    gpci.stageCount = 2; gpci.pStages = stages;
    gpci.pVertexInputState = &vis; gpci.pInputAssemblyState = &ias; gpci.pViewportState = &vps;
    gpci.pRasterizationState = &rs; gpci.pMultisampleState = &ms; gpci.pDepthStencilState = &ds;
    gpci.pColorBlendState = &cbs; gpci.pDynamicState = &dsi;
    gpci.layout = S.layout; gpci.renderPass = S.renderPass; gpci.subpass = 0; gpci.basePipelineIndex = -1;
    if (S.f.createPipelines(g_ctx.dev, 0, 1, &gpci, nullptr, &S.pipeline) != 0) return fail(err, "vkCreateGraphicsPipelines failed");
    return true;
}

// creates everything the render path needs; no-op once it exists for the current device
bool ensure_render(const char** err) {
    if (S.ready) return true;
    if (!g_ctx.ready || !g_ctx.dev || !g_ctx.getDevProc) return fail(err, "Vulkan context is not ready");
    if (!load_functions(err)) return false;
    if (!create_pipeline(err)) return false;
    // command buffer and fence for the render path (the fence starts signaled so the first wait returns)
    CommandBufferAllocateInfo cbai = {};
    cbai.sType = S_COMMAND_BUFFER_ALLOC; cbai.commandPool = g_ctx.pool; cbai.level = 0; cbai.commandBufferCount = 1;
    if (g_ctx.f.allocCmd(g_ctx.dev, &cbai, &S.cmd) != 0 || !S.cmd) return fail(err, "vkAllocateCommandBuffers failed");
    FenceCreateInfo fci = {};
    fci.sType = S_FENCE; fci.flags = FENCE_SIGNALED;
    if (g_ctx.f.createFence(g_ctx.dev, &fci, nullptr, &S.fence) != 0) return fail(err, "vkCreateFence failed");
    S.ready = true;
    return true;
}

void destroy_target() {
    Target& t = S.target;
    if (!t.w) return;
    if (t.fb) S.f.destroyFramebuffer(g_ctx.dev, t.fb, nullptr);
    if (t.colorView) S.f.destroyImageView(g_ctx.dev, t.colorView, nullptr);
    if (t.depthView) S.f.destroyImageView(g_ctx.dev, t.depthView, nullptr);
    if (t.color) S.f.destroyImage(g_ctx.dev, t.color, nullptr);
    if (t.depth) S.f.destroyImage(g_ctx.dev, t.depth, nullptr);
    if (t.colorMem) S.f.freeMemory(g_ctx.dev, t.colorMem, nullptr);
    if (t.depthMem) S.f.freeMemory(g_ctx.dev, t.depthMem, nullptr);
    if (t.rbuf) S.f.destroyBuffer(g_ctx.dev, t.rbuf, nullptr);
    if (t.rbufMem) S.f.freeMemory(g_ctx.dev, t.rbufMem, nullptr);
    t = Target();
}

// render target of size w x h, recreated when the size changes
bool ensure_target(uint32_t w, uint32_t h, const char** err) {
    Target& t = S.target;
    if (t.w == w && t.h == h && t.fb) return true;
    destroy_target();
    if (!make_image(w, h, FORMAT_B8G8R8A8_UNORM, IMAGE_USAGE_COLOR_ATTACHMENT | IMAGE_USAGE_TRANSFER_SRC, ASPECT_COLOR,
                    t.color, t.colorMem, t.colorView, err)) return false;
    if (!make_image(w, h, FORMAT_D32_SFLOAT, IMAGE_USAGE_DEPTH_ATTACHMENT, ASPECT_DEPTH, t.depth, t.depthMem, t.depthView, err)) return false;
    H views[2] = {t.colorView, t.depthView};
    FramebufferCreateInfo fbci = {};
    fbci.sType = S_FRAMEBUFFER;
    fbci.renderPass = S.renderPass; fbci.attachmentCount = 2; fbci.pAttachments = views;
    fbci.width = w; fbci.height = h; fbci.layers = 1;
    if (S.f.createFramebuffer(g_ctx.dev, &fbci, nullptr, &t.fb) != 0) return fail(err, "vkCreateFramebuffer failed");
    if (!make_buffer((uint64_t)w * h * 4, BUF_USAGE_TRANSFER_DST, t.rbuf, t.rbufMem, err)) return false;
    t.w = w;
    t.h = h;
    return true;
}

// uploads a model's triangles once: positions, normals (0,0,1 when absent) and colors (material color when absent)
GpuMesh* ensure_mesh(const Model* m, const char** err) {
    for (int i = 0; i < S.nmeshes; i++) if (S.meshes[i].key == m) return &S.meshes[i];
    if (S.nmeshes >= kMaxMeshes) { fail(err, "too many models on the GPU"); return nullptr; }

    uint32_t nverts = 0, nidx = 0;
    for (int p = 0; p < m->nprims; p++) { nverts += m->prims[p].nverts; nidx += m->prims[p].nidx - m->prims[p].nidx % 3; }
    if (!nverts || !nidx) { fail(err, "model has no triangles"); return nullptr; }

    Vertex* verts = (Vertex*)mem_alloc(sizeof(Vertex) * nverts);
    uint32_t* idx = (uint32_t*)mem_alloc(sizeof(uint32_t) * nidx);
    GpuMesh gm;
    gm.key = m;
    gm.nprims = m->nprims;
    gm.firstIndex = (uint32_t*)mem_alloc(sizeof(uint32_t) * (m->nprims ? m->nprims : 1));
    gm.indexCount = (uint32_t*)mem_alloc(sizeof(uint32_t) * (m->nprims ? m->nprims : 1));
    gm.vertexOffset = (int32_t*)mem_alloc(sizeof(int32_t) * (m->nprims ? m->nprims : 1));
    if (!verts || !idx || !gm.firstIndex || !gm.indexCount || !gm.vertexOffset) {
        mem_free(verts); mem_free(idx); mem_free(gm.firstIndex); mem_free(gm.indexCount); mem_free(gm.vertexOffset);
        fail(err, "out of memory");
        return nullptr;
    }

    uint32_t vbase = 0, ibase = 0;
    for (int p = 0; p < m->nprims; p++) {
        const Primitive& pr = m->prims[p];
        gm.vertexOffset[p] = (int32_t)vbase;
        gm.firstIndex[p] = ibase;
        gm.indexCount[p] = 0;
        float mat[3] = {0.8f, 0.8f, 0.8f};
        if (pr.material >= 0 && pr.material < m->nmats) {
            const math::vec3<float>& c = m->mats[pr.material].color;
            mat[0] = c.x; mat[1] = c.y; mat[2] = c.z;
        }
        for (uint32_t i = 0; i < pr.nverts; i++) {
            Vertex& v = verts[vbase + i];
            v.p[0] = pr.pos[i].x; v.p[1] = pr.pos[i].y; v.p[2] = pr.pos[i].z;
            if (pr.nrm) { v.n[0] = pr.nrm[i].x; v.n[1] = pr.nrm[i].y; v.n[2] = pr.nrm[i].z; }
            else { v.n[0] = 0; v.n[1] = 0; v.n[2] = 1; }
            if (pr.col) { v.c[0] = pr.col[i].x; v.c[1] = pr.col[i].y; v.c[2] = pr.col[i].z; }
            else { v.c[0] = mat[0]; v.c[1] = mat[1]; v.c[2] = mat[2]; }
        }
        uint32_t n = pr.nidx - pr.nidx % 3;
        for (uint32_t i = 0; i < n; i++) idx[ibase + i] = pr.idx[i];      // relative to the primitive's first vertex
        gm.indexCount[p] = pr.nverts ? n : 0;
        vbase += pr.nverts;
        ibase += n;
    }

    uint64_t vbytes = (uint64_t)sizeof(Vertex) * nverts, ibytes = (uint64_t)sizeof(uint32_t) * nidx;
    bool ok = make_buffer(vbytes, BUF_USAGE_VERTEX, gm.vb, gm.vbMem, err) && make_buffer(ibytes, BUF_USAGE_INDEX, gm.ib, gm.ibMem, err) &&
              upload(gm.vbMem, verts, vbytes, err) && upload(gm.ibMem, idx, ibytes, err);
    mem_free(verts);
    mem_free(idx);
    if (!ok) {
        mem_free(gm.firstIndex); mem_free(gm.indexCount); mem_free(gm.vertexOffset);
        return nullptr;
    }
    S.meshes[S.nmeshes] = gm;
    return &S.meshes[S.nmeshes++];
}

// records the render pass: clear, bind pipeline and buffers, one indexed draw per drawable instance
void record_scene(Cmd cmd, const GpuMesh& gm, const Model* m, uint32_t w, uint32_t h, const float viewProj[16],
                  float r, float g, float b) {
    ClearValue clear[2] = {};
    clear[0].raw[0] = bits_of(r); clear[0].raw[1] = bits_of(g); clear[0].raw[2] = bits_of(b); clear[0].raw[3] = bits_of(1.0f);
    clear[1].raw[0] = bits_of(1.0f);
    RenderPassBeginInfo rpbi = {};
    rpbi.sType = S_RENDER_PASS_BEGIN;
    rpbi.renderPass = S.renderPass; rpbi.framebuffer = S.target.fb;
    rpbi.renderArea = Rect2D{{0, 0}, {w, h}};
    rpbi.clearValueCount = 2; rpbi.pClearValues = clear;
    S.f.cmdBeginRenderPass(cmd, &rpbi, SUBPASS_CONTENTS_INLINE);
    Viewport vp = {0, 0, (float)w, (float)h, 0.f, 1.f};
    Rect2D sc = {{0, 0}, {w, h}};
    S.f.cmdSetViewport(cmd, 0, 1, &vp);
    S.f.cmdSetScissor(cmd, 0, 1, &sc);
    S.f.cmdBindPipeline(cmd, BIND_GRAPHICS, S.pipeline);
    H vb = gm.vb;
    uint64_t off = 0;
    S.f.cmdBindVertexBuffers(cmd, 0, 1, &vb, &off);
    S.f.cmdBindIndexBuffer(cmd, gm.ib, 0, INDEX_UINT32);
    for (int i = 0; i < m->ninst; i++) {
        const Instance& in = m->inst[i];
        if (in.prim < 0 || in.prim >= gm.nprims || !gm.indexCount[in.prim]) continue;
        float pc[32];
        for (int k = 0; k < 16; k++) pc[k] = viewProj[k];
        const float* world = &in.world.c[0].x;
        for (int k = 0; k < 16; k++) pc[16 + k] = world[k];
        S.f.cmdPushConstants(cmd, S.layout, STAGE_SHADER_VERTEX | STAGE_SHADER_FRAGMENT, 0, 128, pc);
        S.f.cmdDrawIndexed(cmd, gm.indexCount[in.prim], 1, gm.firstIndex[in.prim], gm.vertexOffset[in.prim], 0);
    }
    S.f.cmdEndRenderPass(cmd);
}

bool begin_cmd(Cmd cmd, const char** err) {
    CommandBufferBeginInfo bi = {};
    bi.sType = S_COMMAND_BUFFER_BEGIN;
    bi.flags = 1;                                       // ONE_TIME_SUBMIT
    if (g_ctx.f.beginCmd(cmd, &bi) != 0) return fail(err, "vkBeginCommandBuffer failed");
    return true;
}

}  // namespace

// ---- public paths ----

bool render_to_pixels(uint32_t w, uint32_t h, const Model* m, const float viewProj[16], float r, float g, float b,
                      uint32_t* out, const char** err) {
    *err = nullptr;
    if (!w || !h || !m || !out) return fail(err, "bad arguments");
    if (!vk_init_context(err)) return false;
    if (!ensure_render(err) || !ensure_target(w, h, err)) return false;
    GpuMesh* gm = ensure_mesh(m, err);
    if (!gm) return false;

    g_ctx.f.waitFences(g_ctx.dev, 1, &S.fence, 1, ~0ull);
    g_ctx.f.resetFences(g_ctx.dev, 1, &S.fence);
    if (!begin_cmd(S.cmd, err)) return false;
    record_scene(S.cmd, *gm, m, w, h, viewProj, r, g, b);
    SubresourceLayers layers = {ASPECT_COLOR, 0, 0, 1};
    BufferImageCopy region = {};
    region.imageSubresource = layers;
    region.imageExtent = Ext3D{w, h, 1};
    S.f.cmdCopyImageToBuffer(S.cmd, S.target.color, LAYOUT_TRANSFER_SRC, S.target.rbuf, 1, &region);
    g_ctx.f.endCmd(S.cmd);

    SubmitInfo si = {};
    si.sType = S_SUBMIT;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &S.cmd;
    if (g_ctx.f.submit(g_ctx.q, 1, &si, S.fence) != 0) return fail(err, "vkQueueSubmit failed");
    g_ctx.f.waitFences(g_ctx.dev, 1, &S.fence, 1, ~0ull);
    if (!download(S.target.rbufMem, out, (uint64_t)w * h * 4, err)) return false;
    return true;
}

bool render_to_window(void* hinst, void* hwnd, uint32_t w, uint32_t h, const Model* m, const float viewProj[16],
                      float r, float g, float b, const char** err) {
    *err = nullptr;
    if (!m || !w || !h) return fail(err, "bad arguments");
    if (!vk_init_context(err)) return false;
    WinSwap* s = vk_window_state(hinst, hwnd, w, h, err);
    if (!s) return false;
    if (!ensure_render(err) || !ensure_target(w, h, err)) return false;
    GpuMesh* gm = ensure_mesh(m, err);
    if (!gm) return false;

    // the slot for this frame: wait until the GPU is done with it (the same frame slots the clear path uses)
    uint32_t slot = s->frame % kFramesInFlight;
    s->frame++;
    g_ctx.f.waitFences(g_ctx.dev, 1, &s->fence[slot], 1, ~0ull);
    g_ctx.f.resetFences(g_ctx.dev, 1, &s->fence[slot]);

    uint32_t idx = 0;
    R res = g_ctx.f.acquire(g_ctx.dev, s->swap, ~0ull, s->semAcquire[slot], 0, &idx);
    if (res == RES_OUT_OF_DATE) {
        if (!vk_make_swapchain(*s, w, h, err)) return false;
        res = g_ctx.f.acquire(g_ctx.dev, s->swap, ~0ull, s->semAcquire[slot], 0, &idx);
    }
    if (res != 0 && res != RES_SUBOPTIMAL) return fail(err, "vkAcquireNextImageKHR failed");
    if (idx >= s->n) return fail(err, "swapchain image index out of range");

    if (!begin_cmd(S.cmd, err)) return false;
    record_scene(S.cmd, *gm, m, w, h, viewProj, r, g, b);
    // copy the rendered color into the swapchain image, then hand the image to presentation
    Range full = {ASPECT_COLOR, 0, 1, 0, 1};
    ImageMemoryBarrier toDst = {};
    toDst.sType = S_IMAGE_BARRIER;
    toDst.oldLayout = LAYOUT_UNDEFINED; toDst.newLayout = LAYOUT_TRANSFER_DST;
    toDst.srcQueueFamilyIndex = 0xFFFFFFFFu; toDst.dstQueueFamilyIndex = 0xFFFFFFFFu;
    toDst.dstAccessMask = ACCESS_TRANSFER_WRITE;
    toDst.image = s->imgs[idx]; toDst.subresourceRange = full;
    g_ctx.f.barrier(S.cmd, STAGE_TOP, vkp::STAGE_TRANSFER, 0, 0, nullptr, 0, nullptr, 1, &toDst);
    ImageCopy region = {};
    region.srcSubresource = SubresourceLayers{ASPECT_COLOR, 0, 0, 1};
    region.dstSubresource = SubresourceLayers{ASPECT_COLOR, 0, 0, 1};
    region.extent = Ext3D{w, h, 1};
    S.f.cmdCopyImage(S.cmd, S.target.color, LAYOUT_TRANSFER_SRC, s->imgs[idx], LAYOUT_TRANSFER_DST, 1, &region);
    ImageMemoryBarrier toPresent = toDst;
    toPresent.srcAccessMask = ACCESS_TRANSFER_WRITE; toPresent.dstAccessMask = 0;
    toPresent.oldLayout = LAYOUT_TRANSFER_DST; toPresent.newLayout = LAYOUT_PRESENT;
    g_ctx.f.barrier(S.cmd, vkp::STAGE_TRANSFER, STAGE_BOTTOM, 0, 0, nullptr, 0, nullptr, 1, &toPresent);
    g_ctx.f.endCmd(S.cmd);

    uint32_t waitStage = vkp::STAGE_TRANSFER;
    SubmitInfo si = {};
    si.sType = S_SUBMIT;
    si.waitSemaphoreCount = 1; si.pWaitSemaphores = &s->semAcquire[slot]; si.pWaitDstStageMask = &waitStage;
    si.commandBufferCount = 1; si.pCommandBuffers = &S.cmd;
    si.signalSemaphoreCount = 1; si.pSignalSemaphores = &s->semRender[idx];
    if (g_ctx.f.submit(g_ctx.q, 1, &si, s->fence[slot]) != 0) return fail(err, "vkQueueSubmit failed");
    S.f.queueWaitIdle(g_ctx.q);                         // the target and the render buffer are reused next frame

    PresentInfo pi = {};
    pi.sType = S_PRESENT;
    pi.waitSemaphoreCount = 1; pi.pWaitSemaphores = &s->semRender[idx];
    pi.swapchainCount = 1; pi.pSwapchains = &s->swap; pi.pImageIndices = &idx;
    R pr = g_ctx.f.present(g_ctx.q, &pi);
    if (pr != 0 && pr != RES_SUBOPTIMAL && pr != RES_OUT_OF_DATE) return fail(err, "vkQueuePresentKHR failed");
    return true;
}

void free_model_gpu(const Model* m) {
    if (!m) return;
    for (int i = 0; i < S.nmeshes; i++) {
        if (S.meshes[i].key != m) continue;
        GpuMesh& gm = S.meshes[i];
        if (S.ready && g_ctx.dev) {
            g_ctx.f.waitFences(g_ctx.dev, 1, &S.fence, 1, ~0ull);
            if (gm.vb) S.f.destroyBuffer(g_ctx.dev, gm.vb, nullptr);
            if (gm.vbMem) S.f.freeMemory(g_ctx.dev, gm.vbMem, nullptr);
            if (gm.ib) S.f.destroyBuffer(g_ctx.dev, gm.ib, nullptr);
            if (gm.ibMem) S.f.freeMemory(g_ctx.dev, gm.ibMem, nullptr);
        }
        mem_free(gm.firstIndex);
        mem_free(gm.indexCount);
        mem_free(gm.vertexOffset);
        S.meshes[i] = S.meshes[S.nmeshes - 1];
        S.meshes[S.nmeshes - 1] = GpuMesh();
        S.nmeshes--;
        return;
    }
}

void render_shutdown() {
    if (!S.ready) {
        S = State();
        return;
    }
    if (g_ctx.dev) {
        g_ctx.f.deviceWaitIdle(g_ctx.dev);
        for (int i = 0; i < S.nmeshes; i++) {
            GpuMesh& gm = S.meshes[i];
            if (gm.vb) S.f.destroyBuffer(g_ctx.dev, gm.vb, nullptr);
            if (gm.vbMem) S.f.freeMemory(g_ctx.dev, gm.vbMem, nullptr);
            if (gm.ib) S.f.destroyBuffer(g_ctx.dev, gm.ib, nullptr);
            if (gm.ibMem) S.f.freeMemory(g_ctx.dev, gm.ibMem, nullptr);
            mem_free(gm.firstIndex);
            mem_free(gm.indexCount);
            mem_free(gm.vertexOffset);
        }
        destroy_target();
        if (S.fence) g_ctx.f.destroyFence(g_ctx.dev, S.fence, nullptr);
        if (S.pipeline) S.f.destroyPipeline(g_ctx.dev, S.pipeline, nullptr);
        if (S.renderPass) S.f.destroyRenderPass(g_ctx.dev, S.renderPass, nullptr);
        if (S.layout) S.f.destroyPipelineLayout(g_ctx.dev, S.layout, nullptr);
        if (S.vs) S.f.destroyShaderModule(g_ctx.dev, S.vs, nullptr);
        if (S.fs) S.f.destroyShaderModule(g_ctx.dev, S.fs, nullptr);
    }
    S = State();                                        // the command buffer belongs to the pool, freed with the device
}

}}  // namespace evgx::vkp
