// vk_present: presents a red window through the GPU for ~6 seconds, reports frame rate, then exits.
#include <evg/evg.hpp>
#include <stdio.h>

namespace evgx { namespace vkp { uint32_t present_mode(); } }

int main() {
    evgx::Window* w = evg.createwindow(640, 480, "evg vulkan present");
    if (!w) { printf("window: %s\n", evg.error()); return 1; }
    printf("hwnd=%p\n", w->native); fflush(stdout);
    double t0 = evg.time();
    int frames = 0;
    double worst = 0, prev = t0, worst_at = 0;
    while (w->open() && evg.time() - t0 < 6.0) {
        evgx::Event e;
        while (evg.poll_event(e)) {}
        if (!evg.vulkan_clear(w, 0.9f, 0.1f, 0.1f)) { printf("present failed: %s\n", evg.error()); return 2; }
        double now = evg.time();
        if (frames > 0 && now - prev > worst) { worst = now - prev; worst_at = now - t0; }
        prev = now;
        frames++;
    }
    double secs = evg.time() - t0;
    printf("gpu: %s\n", evg.gpu());
    const char* modeName[] = {"IMMEDIATE (no vsync wait)", "MAILBOX (no vsync wait)", "FIFO (vsync-paced)"};
    printf("present mode: %s\n", modeName[evgx::vkp::present_mode() < 3 ? evgx::vkp::present_mode() : 2]);
    printf("presented %d frames in %.2f s = %.1f fps, avg CPU per frame %.3f ms, worst gap %.2f ms at %.2f s\n",
           frames, secs, frames / secs, secs * 1000.0 / frames, worst * 1000.0, worst_at);
    evg.destroywindow(w);
    return 0;
}
