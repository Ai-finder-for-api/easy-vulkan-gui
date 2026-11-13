// render_glb in.glb out.bmp [width height [yaw pitch]]  -- headless render of a GLB (works on any platform)
#include <evg/evg.hpp>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc < 3) { printf("usage: render_glb in.glb out.bmp [width height [yaw_deg pitch_deg]]\n"); return 2; }
    int W = argc > 4 ? atoi(argv[3]) : 960, H = argc > 4 ? atoi(argv[4]) : 540;
    evgx::Model* m = evg.loadglb(argv[1]);
    if (!m) { printf("load failed: %s\n", evg.error()); return 1; }
    int tris = 0; for (int i = 0; i < m->ninst; i++) tris += (int)(m->prims[m->inst[i].prim].nidx / 3);
    printf("%s: %d primitives, %d instances, %d triangles, %d materials%s\n", argv[1], m->nprims, m->ninst, tris, m->nmats - 1,
           m->skipped ? " (some primitives skipped)" : "");
    printf("bounds (%.3f %.3f %.3f) .. (%.3f %.3f %.3f)\n", m->bmin.x, m->bmin.y, m->bmin.z, m->bmax.x, m->bmax.y, m->bmax.z);
    evgx::Window* w = evg.createwindow(W, H, "render_glb");
    if (!w) { printf("window failed: %s\n", evg.error()); return 1; }
    evgx::Orbit o; o.frame(m);
    o.yaw = argc > 5 ? (float)atof(argv[5]) * 0.0174533f : 0.6f;
    o.pitch = argc > 6 ? (float)atof(argv[6]) * 0.0174533f : 0.35f;
    double t0 = evg.time();
    evg.begin(w, 0.10f, 0.12f, 0.18f);
    evg.camera(o.camera());
    evg.draw(m);
    evg.end();
    printf("rendered %dx%d in %.1f ms\n", W, H, (evg.time() - t0) * 1000.0);
    if (!evg.screenshot(w, argv[2])) { printf("screenshot failed: %s\n", evg.error()); return 1; }
    return 0;
}
