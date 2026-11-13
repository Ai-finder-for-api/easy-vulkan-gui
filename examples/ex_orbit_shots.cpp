// ex_orbit_shots: orbit a model around its centre and save a screenshot every 60 degrees (headless, no window needed for the file).
#include <stdio.h>
#include <evg/evg.hpp>

int main(int argc, char** argv) {
    if (argc < 2) { printf("usage: ex_orbit_shots model.glb\n"); return 2; }
    evgx::Model* m = evg.loadglb(argv[1]);
    if (!m) { printf("load failed: %s\n", evg.error()); return 1; }
    evgx::Window* w = evg.createwindow(320, 240, "orbit");
    if (!w) { printf("window failed: %s\n", evg.error()); return 1; }

    evgx::Orbit orbit;
    orbit.frame(m);
    int shots = 0;
    for (int deg = 0; deg < 360; deg += 60) {
        orbit.yaw = deg * 0.0174533f;
        orbit.pitch = 0.35f;
        if (!evg.begin(w, 0.10f, 0.12f, 0.18f)) continue;
        evg.camera(orbit.camera());
        evg.draw(m);
        evg.end();
        char name[64];
        snprintf(name, sizeof name, "orbit_%03d.bmp", deg);
        if (evg.screenshot(w, name)) shots++;
    }
    printf("saved %d screenshots\n", shots);
    evg.destroywindow(w);
    evg.freemodel(m);
    return shots == 6 ? 0 : 1;
}
