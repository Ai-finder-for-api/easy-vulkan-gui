// GPU mesh rendering: a one-triangle GLB built in memory covers the whole clip square, so every pixel of the frame is
// the same lit red (about 170 after the shader's lighting). Also draws into a window and checks that models can be
// freed and loaded again. Skips when there is no Vulkan driver.
#include <string.h>
#include "../common/check.hpp"
#include <evg/math.hpp>

namespace {

const char kJson[] =
    "{\"asset\":{\"version\":\"2.0\"},\"scene\":0,\"scenes\":[{\"nodes\":[0]}],\"nodes\":[{\"mesh\":0}],"
    "\"meshes\":[{\"primitives\":[{\"attributes\":{\"POSITION\":0},\"material\":0}]}],"
    "\"materials\":[{\"pbrMetallicRoughness\":{\"baseColorFactor\":[1,0,0,1],\"metallicFactor\":0,\"roughnessFactor\":1}}],"
    "\"accessors\":[{\"bufferView\":0,\"componentType\":5126,\"count\":3,\"type\":\"VEC3\"}],"
    "\"bufferViews\":[{\"buffer\":0,\"byteOffset\":0,\"byteLength\":36}],"
    "\"buffers\":[{\"byteLength\":36}]}";

void put32(uint8_t* p, uint32_t v) { for (int k = 0; k < 4; k++) p[k] = (uint8_t)(v >> (8 * k)); }
void putf(uint8_t* p, float f) { union { float f; uint32_t u; } x; x.f = f; put32(p, x.u); }

// a GLB with one triangle at (-1,-1), (3,-1), (-1,3): together they cover the whole [-1,1] clip square
int build_glb(uint8_t* out, int cap) {
    int jlen = 0;
    while (kJson[jlen]) jlen++;
    int jpad = (jlen + 3) & ~3;
    int total = 12 + 8 + jpad + 8 + 36;
    if (total > cap) return 0;
    put32(out, 0x46546C67u);                      // "glTF"
    put32(out + 4, 2);
    put32(out + 8, (uint32_t)total);
    uint8_t* j = out + 12;
    put32(j, (uint32_t)jpad);
    put32(j + 4, 0x4E4F534Au);                    // "JSON"
    for (int i = 0; i < jpad; i++) j[8 + i] = i < jlen ? (uint8_t)kJson[i] : ' ';
    uint8_t* b = j + 8 + jpad;
    put32(b, 36);
    put32(b + 4, 0x004E4942u);                    // "BIN\0"
    float pos[9] = {-1, -1, 0, 3, -1, 0, -1, 3, 0};
    for (int i = 0; i < 9; i++) putf(b + 8 + 4 * i, pos[i]);
    return total;
}

uint8_t glb[1024];

}  // namespace

int main() {
    using namespace evgx;

    // skip without a GPU, the same way the cleanup test does
    Window* probe = evg.createwindow(32, 32, "probe");
    if (!probe || !evg.vulkan_clear(probe, 0, 0, 0)) {
        printf("SKIP: no Vulkan device (%s)\n", evg.error());
        if (probe) evg.destroywindow(probe);
        return 0;
    }
    evg.destroywindow(probe);

    int n = build_glb(glb, sizeof glb);
    CHECK(n > 0);
    Model* m = evg.loadglb_memory(glb, (size_t)n);
    CHECK(m != nullptr);
    if (!m) return evgtest::finish("vulkan/render");
    CHECK(m->nprims == 1 && m->ninst == 1);

    // pixels: every pixel is covered; red after lighting, green and blue zero, opaque
    const uint32_t W = 64, H = 48;
    static uint32_t px[W * H];
    for (uint32_t i = 0; i < W * H; i++) px[i] = 0;
    math::mat4 identity;
    CHECK(evg.vulkan_render_pixels(W, H, m, identity, px, 0.f, 0.f, 0.f));
    int badColor = 0;
    for (uint32_t i = 0; i < W * H; i++) {
        uint32_t c = px[i];
        int r = (c >> 16) & 255, g = (c >> 8) & 255, b = c & 255, a = c >> 24;
        if (r < 166 || r > 174 || g > 2 || b > 2 || a != 255) badColor++;
    }
    CHECK(badColor == 0);
    if (badColor) printf("first pixel 0x%08X (expected about 0xFFAA0000)\n", px[0]);

    // a second frame of another size: the render target is recreated and the model is reused
    static uint32_t tiny[16 * 8];
    CHECK(evg.vulkan_render_pixels(16, 8, m, identity, tiny, 0.f, 0.f, 0.f));
    CHECK(((tiny[0] >> 16) & 255) >= 166);

    // the clear color only shows where the triangle does not cover: a triangle that is off screen leaves it
    math::mat4 away = math::translate(math::mat4(1.0f), math::vec3<float>(10.f, 0.f, 0.f));
    static uint32_t bg[16 * 8];
    CHECK(evg.vulkan_render_pixels(16, 8, m, away, bg, 0.f, 0.f, 1.f));
    CHECK((bg[0] & 255) == 255 && ((bg[0] >> 16) & 255) == 0);        // blue background, nothing drawn

    // drawing into a window (renders offscreen, copies to the swapchain, presents)
    Window* w = evg.createwindow(64, 48, "vulkan draw");
    CHECK(w != nullptr);
    if (w) {
        CHECK(evg.vulkan_draw(w, m, identity, 0.1f, 0.2f, 0.3f));
        CHECK(evg.vulkan_draw(w, m, identity, 0.1f, 0.2f, 0.3f));       // two frames in a row
        evg.destroywindow(w);
    }

    // freeing a model drops its GPU copy; a fresh load works
    evg.freemodel(m);
    Model* m2 = evg.loadglb_memory(glb, (size_t)n);
    CHECK(m2 != nullptr);
    if (m2) {
        CHECK(evg.vulkan_render_pixels(W, H, m2, identity, px, 0.f, 0.f, 0.f));
        CHECK(((px[0] >> 16) & 255) >= 166);
        evg.freemodel(m2);
    }

    // a null model is an error, not a crash
    CHECK(!evg.vulkan_render_pixels(W, H, nullptr, identity, px));
    return evgtest::finish("vulkan/render");
}
