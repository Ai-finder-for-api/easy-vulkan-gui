#version 450
// Mesh fragment shader: one directional light plus ambient, a fixed direction so results are reproducible.
layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec3 vColor;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 n = normalize(vNormal);
    float diffuse = max(dot(n, normalize(vec3(0.4, 0.8, 0.6))), 0.0);
    outColor = vec4(vColor * (0.25 + 0.75 * diffuse), 1.0);
}
