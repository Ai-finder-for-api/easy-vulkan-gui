// Software frame API: begin/end, clear color, draw outside a frame, closed windows, BMP screenshots.
#include <stdio.h>
#include <string.h>
#include "../common/check.hpp"

#ifndef EVG_ASSET_DIR
#define EVG_ASSET_DIR "tests/assets"
#endif

int main() {
    evgx::Window* w = evg.createwindow(16, 8, "frame");
    CHECK(w != nullptr);
    if (!w) return evgtest::finish("render/frame");

    // begin clears to the color; channels are rounded to 8 bits, alpha is opaque
    CHECK(evg.begin(w, 0.f, 1.f, 0.f));
    evg.end();
    CHECK(w->color[0] == 0xFF00FF00u);
    CHECK(w->color[w->fbw * w->fbh - 1] == 0xFF00FF00u);
    CHECK(evg.begin(w, 2.f, -1.f, 0.5f));                  // out-of-range channels clamp
    evg.end();
    CHECK(w->color[5] == 0xFFFF0080u);

    // drawing outside begin/end does nothing and does not crash
    evgx::Model* m = nullptr;
    char path[512];
    snprintf(path, sizeof path, "%s/floor.glb", EVG_ASSET_DIR);
    m = evg.loadglb(path);
    CHECK(m != nullptr);
    evg.draw(m);                                           // no frame: ignored
    evg.camera(evgx::Camera());                            // no frame: ignored
    CHECK(evg.begin(w, 0.f, 0.f, 0.f));
    evg.draw(nullptr);                                     // null model: ignored
    evg.end();

    // a frame with a model writes at least one non-background pixel
    CHECK(evg.begin(w, 0.f, 0.f, 0.f));
    evg.camera(evgx::Camera());
    evg.draw(m);
    evg.end();
    int lit = 0;
    for (int i = 0; i < w->fbw * w->fbh; i++) if (w->color[i] != 0xFF000000u) lit++;
    CHECK(lit > 0);
    if (m) evg.freemodel(m);

    // screenshot: a valid 24-bit BMP of the window size
    CHECK(evg.screenshot(w, "frame_test.bmp"));
    FILE* f = fopen("frame_test.bmp", "rb");
    CHECK(f != nullptr);
    if (f) {
        unsigned char head[54];
        size_t got = fread(head, 1, 54, f);
        fclose(f);
        CHECK(got == 54);
        CHECK(head[0] == 'B' && head[1] == 'M');
        CHECK(head[28] == 24);                             // bits per pixel
        int bmpW = head[18] | (head[19] << 8), bmpH = head[22] | (head[23] << 8);
        CHECK(bmpW == w->fbw && bmpH == w->fbh);           // the window's real size (Windows may enlarge tiny windows)
    }

    // a closed window cannot begin a frame
    w->closed = true;
    CHECK(!evg.begin(w, 0.f, 0.f, 0.f));
    CHECK(!evg.vulkan_clear(w, 0.f, 0.f, 0.f));
    evg.destroywindow(w);

    return evgtest::finish("render/frame");
}
