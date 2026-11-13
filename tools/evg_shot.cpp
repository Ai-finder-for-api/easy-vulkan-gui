// evg_shot: render a .glb to a BMP with an orbit camera. Usage: evg_shot model.glb out.bmp [--size=WxH] [--yaw=deg] [--pitch=deg]
#include <stdio.h>
#include <evg/evg.hpp>
#include <evg/common/args.hpp>
#include <evg/common/str.hpp>
#include <evg/common/color.hpp>
#include <evg/common/timer.hpp>

using namespace evgx;

int main(int argc, char** argv) {
    static const char* const kFlags[] = {"help"};
    cli::Args args(argc, argv, kFlags);
    if (args.positional_count() < 2 || args.has("help")) {
        printf("usage: evg_shot model.glb out.bmp [--size=640x360] [--yaw=40] [--pitch=20] [--bg=RRGGBB]\n");
        return args.has("help") ? 0 : 2;
    }
    // size as WxH, defaulting to 640x360 for anything malformed
    int W = 640, H = 360;
    if (const char* sz = args.get("size", nullptr)) {
        str::View v(sz);
        size_t x = str::find(v, 'x');
        int64_t w = 0, h = 0;
        if (x != (size_t)-1 && str::to_i64(v.sub(0, x), w) && str::to_i64(v.sub(x + 1), h) && w > 0 && h > 0 && w < 8192 && h < 8192) {
            W = (int)w;
            H = (int)h;
        }
    }
    float yaw = (float)args.get_double("yaw", 40.0) * 0.0174533f;
    float pitch = (float)args.get_double("pitch", 20.0) * 0.0174533f;
    uint32_t bg = 0xFF1E1F2Fu;
    if (const char* b = args.get("bg", nullptr)) {
        uint32_t v = 0;
        bool ok = true;
        for (const char* p = b; *p; p++) {
            char c = *p;
            int d = (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'f') ? c - 'a' + 10 : (c >= 'A' && c <= 'F') ? c - 'A' + 10 : -1;
            if (d < 0) { ok = false; break; }
            v = v * 16 + (uint32_t)d;
        }
        if (ok) bg = 0xFF000000u | v;
    }

    timer::Stopwatch load;
    Model* m = evg.loadglb(args.positional(0));
    if (!m) { fprintf(stderr, "load failed: %s\n", evg.error()); return 1; }
    double loadMs = load.millis();

    Window* w = evg.createwindow(W, H, "evg_shot");
    if (!w) { fprintf(stderr, "window failed: %s\n", evg.error()); return 1; }

    Orbit orbit;
    orbit.frame(m);
    orbit.yaw = yaw;
    orbit.pitch = pitch;
    timer::Stopwatch frame;
    if (!evg.begin(w, color::red(bg) / 255.f, color::green(bg) / 255.f, color::blue(bg) / 255.f)) return 1;
    evg.camera(orbit.camera());
    evg.draw(m);
    evg.end();
    double frameMs = frame.millis();

    int code = 0;
    if (!evg.screenshot(w, args.positional(1))) { fprintf(stderr, "write failed: %s\n", evg.error()); code = 1; }
    else printf("wrote %s (%dx%d) load %.2f ms, frame %.2f ms\n", args.positional(1), W, H, loadMs, frameMs);

    evg.destroywindow(w);
    evg.freemodel(m);
    return code;
}
