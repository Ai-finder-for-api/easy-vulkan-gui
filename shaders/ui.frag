#version 450

layout(location = 0) in  vec2 fragUV;
layout(location = 1) in  vec4 fragColor;

layout(binding = 0) uniform sampler2D fontAtlas;

layout(location = 0) out vec4 outColor;

void main() {
    float alpha = texture(fontAtlas, fragUV).r;
    outColor    = fragColor * vec4(1.0, 1.0, 1.0, alpha);
}
