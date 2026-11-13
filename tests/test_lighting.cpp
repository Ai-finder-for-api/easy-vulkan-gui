#include <evg/evg.hpp>
#include <evg/lighting.hpp>
#include <stdio.h>
#include <stdlib.h>
static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s   [%s]\n", __LINE__, #c, evg.error()); fails++; } } while (0)
static bool near_f(float a, float b, float e) { return (a > b ? a - b : b - a) <= e; }

int main() {
    using namespace evgx; using namespace evgx::math;
    // ---- uniform white environment of radiance 1: irradiance/pi must be ~1 in every direction ----
    {
        const int W = 128, H = 64;
        float* img = (float*)malloc(sizeof(float) * W * H * 3);
        for (int i = 0; i < W * H * 3; i++) img[i] = 1.f;
        Lighting L;
        CHECK(lighting_build(&L, img, W, H));
        vec3<float> dirs[4] = {vec3<float>(0,1,0), vec3<float>(0,-1,0), vec3<float>(1,0,0), normalize(vec3<float>(1,2,-3))};
        for (int k = 0; k < 4; k++) {
            vec3<float> e = lighting_irradiance(L, dirs[k]);
            printf("uniform env irradiance/pi along dir %d: %.3f %.3f %.3f\n", k, e.x, e.y, e.z);
            CHECK(near_f(e.x, 1.f, 0.06f) && near_f(e.y, 1.f, 0.06f) && near_f(e.z, 1.f, 0.06f));
        }
        vec3<float> s = lighting_sample(L, normalize(vec3<float>(0.3f, 0.5f, 0.8f)), 0.7f);
        CHECK(near_f(s.x, 1.f, 1e-3f) && near_f(s.y, 1.f, 1e-3f));
        lighting_free(&L);
        free(img);
    }
    // ---- a bright sun in +X must light the +X face most, -X least ----
    {
        const int W = 256, H = 128;
        float* img = (float*)calloc((size_t)W * H * 3, sizeof(float));
        for (int y = 0; y < H; y++) for (int x = 0; x < W; x++) {
            float u = (x + 0.5f) / W, v = (y + 0.5f) / H;
            vec3<float> d(sin(v * pi) * cos((u - 0.5f) * 2 * pi), cos(v * pi), sin(v * pi) * sin((u - 0.5f) * 2 * pi));
            float sun = max(dot(d, normalize(vec3<float>(1, 0.3f, 0))), 0.f);
            float v0 = 0.1f + 20.f * sun * sun * sun * sun * sun * sun * sun * sun;
            img[((size_t)y * W + x) * 3 + 0] = v0; img[((size_t)y * W + x) * 3 + 1] = v0; img[((size_t)y * W + x) * 3 + 2] = v0;
        }
        Lighting L; CHECK(lighting_build(&L, img, W, H));
        float px = lighting_irradiance(L, vec3<float>(1, 0, 0)).x, nx = lighting_irradiance(L, vec3<float>(-1, 0, 0)).x;
        printf("sun: irradiance +X %.3f  -X %.3f\n", px, nx);
        CHECK(px > 3.f * nx);
        CHECK(lighting_sample(L, normalize(vec3<float>(1, 0.3f, 0)), 0.f).x > 10.f);   // mirror sample sees the sun
        CHECK(lighting_sample(L, normalize(vec3<float>(1, 0.3f, 0)), 1.f).x < lighting_sample(L, normalize(vec3<float>(1, 0.3f, 0)), 0.f).x);  // rougher = blurrier = dimmer peak
        lighting_free(&L);
        free(img);
    }
    // ---- public API: raw import, render with IBL, clear ----
    {
        const int W = 64, H = 32;
        float* img = (float*)malloc(sizeof(float) * W * H * 3);
        for (int i = 0; i < W * H * 3; i++) img[i] = 0.5f;
        Window* w = evg.createwindow(120, 90, "ibl");
        CHECK(w != nullptr);
        CHECK(evg.import_lighting_raw(img, W, H) && evg.has_lighting());
        CHECK(evg.begin(w, 0, 0, 0)); evg.camera(Camera()); evg.end();
        evg.clear_lighting(); CHECK(!evg.has_lighting());
        free(img);
        evg.destroywindow(w);
    }
    // ---- without the EXR module, import_lighting(file) explains itself ----
    {
        bool ok = evg.import_lighting("sky.exr");
#ifdef EVG_WITH_EXR
        printf("import_lighting(missing file) -> %s : %s\n", ok ? "true" : "false", evg.error());
        CHECK(!ok);
#else
        CHECK(!ok && evg.error()[0] != 0);
        printf("import_lighting without EXR module -> %s\n", evg.error());
#endif
    }
    printf(fails ? "lighting: %d FAILED\n" : "lighting: all passed\n", fails);
    return fails != 0;
}
