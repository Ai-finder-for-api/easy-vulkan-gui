// SDL2 shim: key codes and scancodes, event translation through SDL_PollEvent, keyboard and mouse state, timing.
#include "../common/check.hpp"
#include <evg/compat/SDL.h>

int main() {
    evgx::Window* w = evg.createwindow(64, 48, "sdl");
    CHECK(w != nullptr);
    if (!w) return evgtest::finish("compat/sdl");

    SDL_Event se;
    while (SDL_PollEvent(&se)) {}                  // drain window-creation events

    // letters: keycode is lowercase ASCII, scancode is the letter's SDL number
    w->on_key('A', true);
    CHECK(SDL_PollEvent(&se) == 1 && se.type == SDL_KEYDOWN);
    CHECK(se.key.keysym.sym == SDLK_a && se.key.keysym.scancode == SDL_SCANCODE_A);
    CHECK(se.key.state == SDL_PRESSED && se.key.repeat == 0);
    CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A] == 1);
    w->on_key('A', false);
    CHECK(SDL_PollEvent(&se) == 1 && se.type == SDL_KEYUP);
    CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A] == 0);

    // digits and arrows
    w->on_key('0', true);
    CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_0] == 1);
    w->on_key('0', false);
    while (SDL_PollEvent(&se)) {}
    w->on_key(evgx::key::Right, true);
    CHECK(SDL_PollEvent(&se) == 1 && se.key.keysym.sym == SDLK_RIGHT && se.key.keysym.scancode == SDL_SCANCODE_RIGHT);
    CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RIGHT] == 1);
    w->on_key(evgx::key::Right, false);
    while (SDL_PollEvent(&se)) {}
    CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_UP] == 0);

    // mouse button and wheel translation
    w->on_mouse_move(20, 21);
    while (SDL_PollEvent(&se)) {}
    w->on_button(2, true);
    CHECK(SDL_PollEvent(&se) == 1 && se.type == SDL_MOUSEBUTTONDOWN && se.button.button == SDL_BUTTON_MIDDLE);
    w->on_button(2, false);
    while (SDL_PollEvent(&se)) {}
    w->on_wheel(3.f);
    CHECK(SDL_PollEvent(&se) == 1 && se.type == SDL_MOUSEWHEEL && se.wheel.y == 3);

    // mouse state: masks and position
    w->on_button(0, true);
    w->on_button(1, true);
    int mx = 0, my = 0;
    Uint32 mask = SDL_GetMouseState(&mx, &my);
    CHECK(mask == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK));
    CHECK(mx == 20 && my == 21);
    w->on_button(0, false);
    w->on_button(1, false);
    while (SDL_PollEvent(&se)) {}

    // relative motion is read once: the second read is zero
    SDL_GetRelativeMouseState(nullptr, nullptr);
    w->on_mouse_move(25, 19);
    int rx = 0, ry = 0;
    SDL_GetRelativeMouseState(&rx, &ry);
    CHECK(rx == 5 && ry == -2);
    SDL_GetRelativeMouseState(&rx, &ry);
    CHECK(rx == 0 && ry == 0);

    // relative mouse mode flag and cursor calls
    CHECK(SDL_SetRelativeMouseMode(SDL_TRUE) == 0 && SDL_GetRelativeMouseMode() == SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_FALSE);
    CHECK(SDL_GetRelativeMouseMode() == SDL_FALSE);
    CHECK(SDL_ShowCursor(-1) == 1);

    // size query: matches the window's real client size (Windows may enlarge tiny requested sizes)
    int ww = 0, wh = 0;
    SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(w), &ww, &wh);
    CHECK(ww == w->width && wh == w->height && ww > 0 && wh > 0);
    CHECK(SDL_GetTicks() < 1000000u);
    CHECK(SDL_GetPerformanceFrequency() > 0 && SDL_GetPerformanceCounter() > 0);

    // quit: closing the window yields SDL_QUIT once (after the queued input is read)
    while (SDL_PollEvent(&se)) {}
    w->closed = true;
    CHECK(SDL_PollEvent(&se) == 1 && se.type == SDL_QUIT);
    CHECK(SDL_PollEvent(&se) == 0);
    SDL_DestroyWindow(reinterpret_cast<SDL_Window*>(w));

    return evgtest::finish("compat/sdl");
}
