// SDL2 window basics: init, quit, errors, create/destroy, size, title, delay. Windows themselves are evg windows.
#include "sdl_internal.hpp"

extern "C" {
int SDL_Init(Uint32) {
    evgx::sdl::since_start();                              // start the tick clock
    return evg.init() ? 0 : -1;
}

void SDL_Quit(void) {}

const char* SDL_GetError(void) { return evg.error(); }

SDL_Window* SDL_CreateWindow(const char* title, int, int, int w, int h, Uint32) {
    return reinterpret_cast<SDL_Window*>(evg.createwindow(w, h, title));
}

void SDL_DestroyWindow(SDL_Window* w) { evg.destroywindow(reinterpret_cast<evgx::Window*>(w)); }

void SDL_GetWindowSize(SDL_Window* w, int* width, int* height) {
    evgx::Window* ww = reinterpret_cast<evgx::Window*>(w);
    if (width) *width = ww ? ww->width : 0;
    if (height) *height = ww ? ww->height : 0;
}

void SDL_SetWindowTitle(SDL_Window*, const char*) {}

void SDL_Delay(Uint32 ms) { evg.sleep((int)ms); }
}
