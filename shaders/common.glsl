// ── vgui common shader utilities ──────────────────────────────────

#ifndef VGUI_COMMON_GLSL
#define VGUI_COMMON_GLSL

// Standard vertex input locations
#define VGUI_LOC_POSITION  0
#define VGUI_LOC_NORMAL    1
#define VGUI_LOC_UV        2
#define VGUI_LOC_TANGENT   3
#define VGUI_LOC_BONE_ID   4
#define VGUI_LOC_BONE_W    5

// Standard descriptor set / binding slots
#define VGUI_SET_PER_FRAME    0
#define VGUI_SET_PER_MATERIAL 1
#define VGUI_SET_PER_OBJECT   2

#define VGUI_BIND_MVP       0
#define VGUI_BIND_ALBEDO    0
#define VGUI_BIND_NORMAL    1
#define VGUI_BIND_MR        2
#define VGUI_BIND_AO        3
#define VGUI_BIND_EMISSIVE  4

// Tonemapping – Reinhard
vec3 tonemap_reinhard(vec3 hdr) { return hdr / (hdr + vec3(1.0)); }

// Gamma correct
vec3 linear_to_srgb(vec3 c) { return pow(clamp(c, 0.0, 1.0), vec3(1.0 / 2.2)); }
vec3 srgb_to_linear(vec3 c) { return pow(c, vec3(2.2)); }

#endif // VGUI_COMMON_GLSL
