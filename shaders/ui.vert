#version 450

layout(location = 0) in  vec2 inPos;
layout(location = 1) in  vec2 inUV;
layout(location = 2) in  vec4 inColor;

layout(push_constant) uniform PushConstants {
    vec2 screen_size;
} pc;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec4 fragColor;

void main() {
    // Convert pixel coordinates to NDC
    vec2 ndc = (inPos / pc.screen_size) * 2.0 - 1.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
    fragUV      = inUV;
    fragColor   = inColor;
}
