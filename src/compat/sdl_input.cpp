// SDL2 input state queries: keyboard snapshot, mouse position and buttons, relative mouse motion, relative-mode flag.
#include "sdl_internal.hpp"

namespace evgx { namespace sdl {

bool g_relative = false;

Window* mouse_window() {
    Window* w = evg.focus_window();
    return w ? w : evg.window_at(0);
}

}}  // namespace evgx::sdl

extern "C" {
SDL_bool SDL_IsTextInputActive(void) { return SDL_FALSE; }

int SDL_SetRelativeMouseMode(SDL_bool enabled) {
    evgx::sdl::g_relative = enabled == SDL_TRUE;
    return 0;
}

SDL_bool SDL_GetRelativeMouseMode(void) { return evgx::sdl::g_relative ? SDL_TRUE : SDL_FALSE; }

int SDL_ShowCursor(int toggle) { return toggle < 0 ? 1 : toggle; }

// keyboard state is the union over all open windows (keys are held per window)
const Uint8* SDL_GetKeyboardState(int* numkeys) {
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) evgx::sdl::g_keystate[i] = 0;
    int n = evg.window_count();
    for (int wi = 0; wi < n; wi++) {
        evgx::Window* w = evg.window_at(wi);
        if (!w) continue;
        for (int vk = 0; vk < 256; vk++)
            if (w->keys[vk]) {
                int sc = evgx::sdl::scancode_of(vk);
                if (sc) evgx::sdl::g_keystate[sc] = 1;
            }
    }
    if (numkeys) *numkeys = SDL_NUM_SCANCODES;
    return evgx::sdl::g_keystate;
}

Uint32 SDL_GetMouseState(int* x, int* y) {
    evgx::Window* w = evgx::sdl::mouse_window();
    if (x) *x = w ? w->mouse_x : 0;
    if (y) *y = w ? w->mouse_y : 0;
    Uint32 m = 0;
    if (w) {
        if (w->buttons[0]) m |= SDL_BUTTON_LMASK;
        if (w->buttons[2]) m |= SDL_BUTTON_MMASK;
        if (w->buttons[1]) m |= SDL_BUTTON_RMASK;
    }
    return m;
}

// SDL semantics: the motion since the previous call; the accumulator is then reset
Uint32 SDL_GetRelativeMouseState(int* x, int* y) {
    evgx::Window* w = evgx::sdl::mouse_window();
    if (x) *x = w ? (int)w->dx : 0;
    if (y) *y = w ? (int)w->dy : 0;
    if (w) { w->dx = 0; w->dy = 0; }
    return SDL_GetMouseState(nullptr, nullptr);
}
}
