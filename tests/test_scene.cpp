#include <evg/evg.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef EVG_ASSET_DIR
#define EVG_ASSET_DIR "tests/assets"
#endif
static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s   [evg error: '%s']\n", __LINE__, #c, evg.error()); fails++; } } while (0)

static void rgb(evgx::Window* w, int x, int y, int* r, int* g, int* b) {
    uint32_t p = w->color[(size_t)y * w->fbw + x];
    *r = (p >> 16) & 255; *g = (p >> 8) & 255; *b = p & 255;
}
static bool near_f(float a, float b, float e = 1e-3f) { return (a > b ? a - b : b - a) <= e; }

static evgx::Model* load(const char* name) {
    char path[512]; snprintf(path, sizeof path, "%s/%s", EVG_ASSET_DIR, name);
    return evg.loadglb(path);
}

int main() {
    using namespace evgx;
    Window* w = evg.createwindow(200, 200, "test");
    CHECK(w != nullptr);
    if (!w) return 1;

    // ---- depth ordering, back-face culling ----
    {
        Model* m = load("depth.glb");
        CHECK(m && m->ninst == 3);
        Camera cam; cam.pos = math::vec3<float>(0.f, 0.f, 5.f);
        CHECK(evg.begin(w, 0.f, 0.f, 0.f));
        evg.camera(cam); evg.draw(m); evg.end();
        int r, g, b;
        rgb(w, 100, 100, &r, &g, &b); printf("center pixel   %3d %3d %3d (expect red)\n", r, g, b);
        CHECK(r > 150 && r > g + 80 && r > b + 80);              // near red quad wins over the far blue quad
        rgb(w, 15, 100, &r, &g, &b); printf("left pixel     %3d %3d %3d (expect blue)\n", r, g, b);
        CHECK(b > 150 && b > r + 80 && b > g + 80);              // far blue quad visible around it
        int green = 0;
        for (int y = 0; y < w->fbh; y++) for (int x = 0; x < w->fbw; x++) { rgb(w, x, y, &r, &g, &b); if (g > r + 20 && g > b + 20) green++; }
        CHECK(green == 0);                                      // back-facing green quad is culled
        evg.freemodel(m);
    }

    // ---- near-plane clipping: floor extends behind the camera ----
    {
        Model* m = load("floor.glb");
        CHECK(m != nullptr);
        Camera cam; cam.pos = math::vec3<float>(0.f, 0.f, 0.f); cam.pitch = -0.35f;
        CHECK(evg.begin(w, 0.2f, 0.3f, 0.5f));
        evg.camera(cam); evg.draw(m); evg.end();
        int r, g, b, floor_px = 0, sky_px = 0;
        for (int y = 0; y < w->fbh; y++) for (int x = 0; x < w->fbw; x++) {
            rgb(w, x, y, &r, &g, &b);
            if (r > 60 && abs(r - g) < 12 && abs(g - b) < 12) floor_px++;
            if (b > r + 30) sky_px++;
        }
        printf("near clip: floor px %d, sky px %d of %d\n", floor_px, sky_px, w->fbw * w->fbh);
        CHECK(floor_px > 15000 && sky_px > 3000);               // floor fills the lower part, sky stays above the horizon
        rgb(w, 100, 190, &r, &g, &b); CHECK(r > 60);            // bottom center is lit floor
        evg.freemodel(m);
    }

    // ---- full scene: counts, framing, shading ----
    Model* scene = load("scene.glb");
    CHECK(scene != nullptr);
    if (scene) {
        printf("scene.glb: %d prims, %d instances, %d materials, bounds (%.2f %.2f %.2f)..(%.2f %.2f %.2f)\n", scene->nprims, scene->ninst, scene->nmats,
               scene->bmin.x, scene->bmin.y, scene->bmin.z, scene->bmax.x, scene->bmax.y, scene->bmax.z);
        CHECK(scene->nprims == 5 && scene->ninst == 6 && scene->nmats == 7 && scene->skipped == 0);
        CHECK(scene->bmax.x > scene->bmin.x && scene->bmax.y > scene->bmin.y && scene->bmax.z > scene->bmin.z);
        // the scaled ground is 7 wide at root rotation 17 deg: world x extent must exceed 6
        CHECK(scene->bmax.x - scene->bmin.x > 6.f);

        Window* big = evg.createwindow(960, 540, "scene");
        CHECK(big != nullptr);
        Orbit orbit; orbit.frame(scene); orbit.yaw = 0.5f; orbit.pitch = 0.45f; orbit.dist *= 0.75f;
        double t0 = evg.time();
        const int frames = 5;
        for (int f = 0; f < frames; f++) {
            evg.begin(big, 0.12f, 0.14f, 0.2f);
            evg.camera(orbit.camera());
            evg.draw(scene);
            evg.end();
        }
        double ms = (evg.time() - t0) * 1000.0 / frames;
        printf("render 960x540: %.1f ms/frame (software, single thread)\n", ms);
        int bg = 0, n = big->fbw * big->fbh;
        uint32_t bgc = big->color[0];
        for (int i = 0; i < n; i++) if (big->color[i] == bgc) bg++;
        printf("non-background pixels: %d of %d\n", n - bg, n);
        CHECK(n - bg > n / 10);
        // distinct colors => shading / vertex colors / materials are all contributing
        static uint32_t seen[4096]; int distinct = 0;
        for (int i = 0; i < n; i++) {
            uint32_t p = big->color[i], h = (p * 2654435761u) >> 20; bool dup = false;
            for (int k = 0; k < 4; k++) if (seen[(h + k) & 4095] == p) { dup = true; break; }
            if (!dup) { for (int k = 0; k < 4; k++) if (!seen[(h + k) & 4095]) { seen[(h + k) & 4095] = p; distinct++; break; } }
        }
        printf("distinct colors (approx): %d\n", distinct);
        CHECK(distinct > 300);
        CHECK(evg.screenshot(big, "/tmp/evg_scene.bmp"));
        evg.destroywindow(big);
    }

    // ---- input: scripted keyboard / mouse ----
    {
        Window* iw = evg.createwindow(64, 64, "input");
        iw->on_key('W', true);
        CHECK(iw->key_down('W') && iw->key_pressed('W'));
        iw->open();                                             // new frame: edge flag clears, held state stays
        CHECK(iw->key_down('W') && !iw->key_pressed('W'));
        iw->on_key('W', true, true);                            // auto-repeat must not re-trigger "pressed"
        CHECK(!iw->key_pressed('W'));
        Camera cam; iw->dt = 0.5f;
        cam.fly(*iw, 3.f);                                      // W for 0.5 s at speed 3 => 1.5 units forward (-Z)
        CHECK(near_f(cam.pos.z, 3.5f) && near_f(cam.pos.x, 0.f));
        iw->on_key('W', false); iw->on_key('D', true); iw->on_key(evgx::key::Shift, true);
        cam.pos = math::vec3<float>(0.f, 0.f, 0.f); cam.fly(*iw, 2.f);   // strafe right, shift = x3 => 3 units
        CHECK(near_f(cam.pos.x, 3.f));
        iw->on_key('D', false); iw->on_key(evgx::key::Shift, false);
        iw->on_mouse_move(10, 10); iw->open();                  // first move only sets the reference point
        iw->on_button(1, true); iw->on_mouse_move(110, 60);
        CHECK(near_f(iw->dx, 100.f) && near_f(iw->dy, 50.f));
        float yaw0 = cam.yaw, pitch0 = cam.pitch;
        cam.fly(*iw, 0.f);
        CHECK(near_f(cam.yaw - yaw0, 0.35f, 1e-4f) && near_f(cam.pitch - pitch0, -0.175f, 1e-4f));
        Orbit o; o.dist = 10.f; float y0 = o.yaw;
        iw->on_button(1, false); iw->on_button(0, true); iw->wheel = 2.f; iw->dx = 100.f; iw->dy = 0.f;
        o.update(*iw);
        CHECK(near_f(o.yaw - y0, 1.0f) && near_f(o.dist, 8.1f, 1e-3f));
        // camera math: forward at yaw=90deg points to +X
        Camera c2; c2.yaw = 1.5707963f;
        CHECK(near_f(c2.forward().x, 1.f, 1e-3f) && near_f(c2.forward().z, 0.f, 1e-3f));
        evg.destroywindow(iw);
    }

    // ---- errors and robustness ----
    {
        CHECK(evg.loadglb("/nonexistent/model.glb") == nullptr);
        CHECK(strlen(evg.error()) > 0);
        const char junk[] = "this is definitely not a glb file at all, not even close...";
        CHECK(evg.loadglb_memory(junk, sizeof junk) == nullptr);

        char path[512]; snprintf(path, sizeof path, "%s/scene.glb", EVG_ASSET_DIR);
        FILE* f = fopen(path, "rb"); CHECK(f != nullptr);
        if (f) {
            fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
            unsigned char* buf = (unsigned char*)malloc((size_t)sz);
            CHECK(fread(buf, 1, (size_t)sz, f) == (size_t)sz); fclose(f);
            CHECK(evg.loadglb_memory(buf, (size_t)sz / 2) == nullptr);       // truncated file
            // mutation fuzz: random byte flips anywhere; loader must never crash or hang
            unsigned s = 12345u; int ok_models = 0, rejected = 0;
            unsigned char* mut = (unsigned char*)malloc((size_t)sz);
            for (int iter = 0; iter < 3000; iter++) {
                memcpy(mut, buf, (size_t)sz);
                int flips = 1 + (int)(s % 6);
                for (int k = 0; k < flips; k++) {
                    s = s * 1664525u + 1013904223u;
                    size_t pos = (s >> 8) % ((iter & 1) ? 4096 : (size_t)sz);      // half the runs hit the JSON header region
                    s = s * 1664525u + 1013904223u;
                    mut[pos] = (unsigned char)(s >> 16);
                }
                Model* m = evg.loadglb_memory(mut, (size_t)sz);
                if (m) {
                    ok_models++;
                    // a mutated-but-valid model must also render without crashing
                    if (evg.begin(w, 0, 0, 0)) { evg.camera(Camera()); evg.draw(m); evg.end(); }
                    evg.freemodel(m);
                } else rejected++;
            }
            printf("fuzz: 3000 mutations -> %d loaded+rendered, %d rejected, no crash\n", ok_models, rejected);
            free(mut); free(buf);
        }
    }
    evg.freemodel(scene);
    printf(fails ? "scene: %d FAILED\n" : "scene: all passed\n", fails);
    return fails != 0;
}
