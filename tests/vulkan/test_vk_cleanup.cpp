// GPU cleanup: windows can be created and destroyed in any order, the device is torn down when the last window closes,
// and the context is rebuilt cleanly for the next window. Skips when no Vulkan driver is present.
#include "../common/check.hpp"

int main() {
    evgx::Window* probe = evg.createwindow(64, 48, "probe");
    CHECK(probe != nullptr);
    if (!probe) return evgtest::finish("vulkan/cleanup");
    if (!evg.vulkan_clear(probe, 0, 0, 0)) {
        printf("SKIP: no Vulkan device (%s)\n", evg.error());
        evg.destroywindow(probe);
        return 0;
    }
    evg.destroywindow(probe);

    // many cycles: the context is created and torn down each time the last window closes
    for (int cycle = 0; cycle < 20; cycle++) {
        evgx::Window* a = evg.createwindow(96, 64, "a");
        evgx::Window* b = evg.createwindow(96, 64, "b");
        evgx::Window* c = evg.createwindow(96, 64, "c");
        CHECK(a && b && c);
        if (!a || !b || !c) break;
        CHECK(evg.vulkan_clear(a, 1, 0, 0));
        CHECK(evg.vulkan_clear(b, 0, 1, 0));
        CHECK(evg.vulkan_clear(c, 0, 0, 1));
        evg.destroywindow(b);                      // middle one first: the table must stay dense
        CHECK(evg.vulkan_clear(a, 0.5f, 0.5f, 0.5f));
        CHECK(evg.vulkan_clear(c, 0, 0, 0));
        evg.destroywindow(a);
        evg.destroywindow(c);                      // last window: device and instance go away here
    }

    // a resize rebuilds the swapchain; the old one must be retired, not leaked, and the window keeps working
    evgx::Window* r = evg.createwindow(64, 64, "resize");
    CHECK(r != nullptr);
    if (r) {
        CHECK(evg.vulkan_clear(r, 0, 0, 1));
        r->on_resize(200, 120);
        CHECK(evg.vulkan_clear(r, 1, 1, 0));
        CHECK(evg.vulkan_clear(r, 0, 1, 1));
        evg.destroywindow(r);
    }

    // a window destroyed without ever touching Vulkan is harmless
    evgx::Window* plain = evg.createwindow(32, 32, "plain");
    CHECK(plain != nullptr);
    evg.destroywindow(plain);
    evg.destroywindow(nullptr);

    return evgtest::finish("vulkan/cleanup");
}
