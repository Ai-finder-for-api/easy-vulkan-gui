// Definitions of the shared core state (see state.hpp) and the framebuffer / queue helpers.
#include "state.hpp"
#include "mem.hpp"

namespace evgx { namespace st {

Window g_pool[kMaxWindows];
bool   g_used[kMaxWindows];
Model* g_models[kMaxModels];
char   g_err[256];
char   g_gpu[256];
bool   g_inited = false, g_init_ok = false;

vk::Instance g_instance = nullptr;
vk::PFN_GetInstanceProcAddr g_gipa = nullptr;

Window*    g_cur = nullptr;
FrameState g_fs;
bool       g_have_cam = false;

Event   g_q[kQueue];
int     g_qh = 0, g_qn = 0;
Window* g_focus = nullptr;

Lighting g_env;
bool     g_env_on = false;

void push_event(const Event& e) {
    if (e.window) g_focus = e.window;
    if (g_qn >= kQueue) return;
    g_q[(g_qh + g_qn) % kQueue] = e;
    g_qn++;
}

void fb_free(Window* w) {
    mem_free(w->color); mem_free(w->depth);
    w->color = nullptr; w->depth = nullptr;
    w->fbw = w->fbh = 0;
}

bool fb_ensure(Window* w) {
    if (w->width <= 0 || w->height <= 0) return false;
    if (w->fbw == w->width && w->fbh == w->height && w->color) return true;
    fb_free(w);
    w->color = alloc_arr<uint32_t>((size_t)w->width * w->height);
    w->depth = alloc_arr<float>((size_t)w->width * w->height);
    if (!w->color || !w->depth) { fb_free(w); return fail("out of memory (framebuffer)"); }
    w->fbw = w->width; w->fbh = w->height;
    return true;
}

}}  // namespace evgx::st
