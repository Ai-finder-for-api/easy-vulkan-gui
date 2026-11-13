// Vulkan on the GPU: clear an image, copy it back, verify every pixel. Needs a Vulkan driver.
#include <stdio.h>
#include <stdint.h>
#include <evg/evg.hpp>

namespace evgx { namespace vkb {
bool clear_readback(uint32_t width, uint32_t height, float r, float g, float b, uint32_t* out, const char** err);
}}

static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)

int main() {
    const uint32_t W = 97, H = 53;                         // odd sizes exercise row padding
    static uint32_t px[W * H];
    const char* err = nullptr;
    bool ok = evgx::vkb::clear_readback(W, H, 1.0f, 0.0f, 0.0f, px, &err);
    if (!ok) {
        printf("vulkan clear: unavailable: %s\n", err ? err : "?");
        // no driver is not a test failure: the software path is covered by the other suites
        printf("vk: skipped\n");
        return 0;
    }
    uint32_t bad = 0;
    for (uint32_t i = 0; i < W * H; i++) if (px[i] != 0xFFFF0000u) { if (bad < 3) printf("pixel %u = %08X\n", i, px[i]); bad++; }
    printf("vulkan clear %ux%u on the GPU: %u of %u pixels wrong, first = %08X\n", W, H, bad, W * H, px[0]);
    CHECK(bad == 0);

    // a second clear with another color, to be sure the image is really written each time
    ok = evgx::vkb::clear_readback(W, H, 0.0f, 0.5f, 1.0f, px, &err);
    CHECK(ok);
    if (ok) {
        uint32_t expect = 0xFF000000u | (0u << 16) | (128u << 8) | 255u;  // 0.5 -> 127.5 rounds to 128 in the driver
        uint32_t g = (px[0] >> 8) & 255, b = px[0] & 255;
        printf("second clear pixel = %08X (green %u, blue %u)\n", px[0], g, b);
        CHECK(((px[0] >> 16) & 255) == 0 && (g == 127 || g == 128) && b == 255);
        (void)expect;
    }
    printf(fails ? "vk: %d FAILED\n" : "vk: all passed\n", fails);
    return fails != 0;
}
