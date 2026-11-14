#version 450
// Mesh vertex shader: transforms a vertex to clip space and passes normal and color to the fragment stage.
// Push constants (128 bytes, the minimum the Vulkan spec guarantees): view-projection and model matrices.
layout(push_constant) uniform Push {
    mat4 viewProj;
    mat4 model;
} pc;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec3 vColor;

void main() {
    vec4 world = pc.model * vec4(inPos, 1.0);
    vNormal = mat3(pc.model) * inNormal;
    vColor = inColor;
    gl_Position = pc.viewProj * world;
}
