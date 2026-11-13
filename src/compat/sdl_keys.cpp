// SDL2 key tables: evg key codes to SDL scancodes and keycodes (letters, digits, control keys, arrows, modifiers).
#include "sdl_internal.hpp"

namespace evgx { namespace sdl {

Uint8 g_keystate[SDL_NUM_SCANCODES];

int scancode_of(int vk) {
    if (vk >= 'A' && vk <= 'Z') return SDL_SCANCODE_A + (vk - 'A');
    if (vk >= '1' && vk <= '9') return SDL_SCANCODE_1 + (vk - '1');
    if (vk == '0') return SDL_SCANCODE_0;
    switch (vk) {
        case 13: return SDL_SCANCODE_RETURN;
        case 27: return SDL_SCANCODE_ESCAPE;
        case 8: return SDL_SCANCODE_BACKSPACE;
        case 9: return SDL_SCANCODE_TAB;
        case 32: return SDL_SCANCODE_SPACE;
        case 39: return SDL_SCANCODE_RIGHT;
        case 37: return SDL_SCANCODE_LEFT;
        case 40: return SDL_SCANCODE_DOWN;
        case 38: return SDL_SCANCODE_UP;
        case 17: return SDL_SCANCODE_LCTRL;
        case 16: return SDL_SCANCODE_LSHIFT;
        case 18: return SDL_SCANCODE_LALT;
    }
    return 0;
}

int keycode_of(int vk) {
    if (vk >= 'A' && vk <= 'Z') return vk + 32;                  // lowercase letter
    if (vk >= '0' && vk <= '9') return vk;
    int sc = scancode_of(vk);
    if (sc == SDL_SCANCODE_RETURN) return SDLK_RETURN;
    if (sc == SDL_SCANCODE_ESCAPE) return SDLK_ESCAPE;
    if (sc == SDL_SCANCODE_BACKSPACE) return SDLK_BACKSPACE;
    if (sc == SDL_SCANCODE_TAB) return SDLK_TAB;
    if (sc == SDL_SCANCODE_SPACE) return SDLK_SPACE;
    return SDLK_SCANCODE_MASK | sc;                              // non-character keys carry the scancode bit
}

}}  // namespace evgx::sdl
