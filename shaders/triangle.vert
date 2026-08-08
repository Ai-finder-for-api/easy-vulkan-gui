#version 450

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inNormal;
layout(location = 2) in  vec2 inUV;

layout(location = 0) out vec3 outColor;

void main() {
    // Map positions to clip space directly (no MVP for this simple example)
    gl_Position = vec4(inPosition, 1.0);
    // Colour from position for visual interest
    outColor = inPosition * 0.5 + 0.5;
}
