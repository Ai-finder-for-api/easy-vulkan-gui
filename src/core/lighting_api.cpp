// Environment lighting API: import_lighting (OpenEXR, optional module), import_lighting_raw, clear, query.
#include "state.hpp"
#include "mem.hpp"

namespace evgx {
using namespace st;

bool Api::import_lighting_raw(const float* rgb, int w, int h) {
    if (g_env_on) { lighting_free(&g_env); g_env_on = false; }
    if (!lighting_build(&g_env, rgb, w, h)) return fail("lighting build failed (out of memory?)");
    g_env_on = true;
    g_err[0] = 0;
    return true;
}

void Api::clear_lighting() {
    if (g_env_on) lighting_free(&g_env);
    g_env_on = false;
}

bool Api::has_lighting() const { return g_env_on; }

#ifdef EVG_WITH_EXR
bool evg_exr_read(const char* path, float** rgb, int* w, int* h, const char** err);   // src/exr
#endif

bool Api::import_lighting(const char* path) {
#ifdef EVG_WITH_EXR
    float* rgb = nullptr; int w = 0, h = 0; const char* err = nullptr;
    if (!evg_exr_read(path, &rgb, &w, &h, &err)) return fail(err ? err : "EXR load failed");
    bool ok = import_lighting_raw(rgb, w, h);
    mem_free(rgb);
    return ok;
#else
    (void)path;
    return fail("import_lighting needs the EXR module: configure with -DEVG_WITH_EXR=ON (uses OpenEXR)");
#endif
}

}  // namespace evgx
