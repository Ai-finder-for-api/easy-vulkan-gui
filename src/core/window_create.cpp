// Window lifetime: createwindow (evg owns it), attach (someone else owns it), destroywindow.
#include "state.hpp"
#include "platform.hpp"

namespace evgx {
namespace vkp { void vk_release_window(void* hwnd); }   // src/vulkan/vk_release.cpp
using namespace st;

Window* Api::createwindow(int width, int height, const char* title) {
    if (width <= 0 || height <= 0) { fail("window size must be positive"); return nullptr; }
    for (int i = 0; i < kMaxWindows; i++) {
        if (g_used[i]) continue;
        Window* w = &g_pool[i];
        *w = Window();
        w->width = width; w->height = height; w->closed = false;
        const char* err = nullptr;
        if (!plat::window_create(w, width, height, title, &err)) { fail(err ? err : "window creation failed"); return nullptr; }
        if (!fb_ensure(w)) { plat::window_destroy(w); return nullptr; }
        g_used[i] = true;
        g_err[0] = 0;
        return w;
    }
    fail("too many windows");
    return nullptr;
}

Window* Api::attach(void* native, int width, int height) {
    if (!native || width <= 0 || height <= 0) { fail("attach needs a native handle and a positive size"); return nullptr; }
    for (int i = 0; i < kMaxWindows; i++) {
        if (g_used[i]) continue;
        Window* w = &g_pool[i];
        *w = Window();
        w->width = width; w->height = height; w->closed = false; w->native = native; w->external = true;
        plat::prepare_external(native);
        if (!fb_ensure(w)) return nullptr;
        g_used[i] = true;
        g_err[0] = 0;
        return w;
    }
    fail("too many windows");
    return nullptr;
}

void Api::destroywindow(Window* w) {
    if (!w) return;
    for (int i = 0; i < kMaxWindows; i++)
        if (&g_pool[i] == w && g_used[i]) {
            vkp::vk_release_window(w->native);             // GPU objects first: they refer to the native window
            if (!w->external) plat::window_destroy(w);
            fb_free(w);
            g_used[i] = false;
            w->closed = true;
            if (g_cur == w) g_cur = nullptr;
            return;
        }
}

}  // namespace evgx
