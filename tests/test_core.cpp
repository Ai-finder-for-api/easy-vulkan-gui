#include <evg/evg.hpp>
#include <evg/compat/SDL.h>
#include <stdio.h>
static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)
int main() {
    // Vulkan probe: on a machine without a GPU driver this must fail cleanly (message, no crash).
    bool ok = evg.init();
    printf("vulkan probe: %s | gpu: '%s' | error: '%s'\n", ok ? "ok" : "unavailable", evg.gpu(), evg.error());
    CHECK(ok || evg.error()[0] != 0);

    // The software path needs no Vulkan: windows always work.
    evgx::Window* w = evg.createwindow(320, 240, "t");
    CHECK(w && w->width == 320 && w->height == 240 && w->fbw == 320 && w->fbh == 240);
    CHECK(evg.createwindow(0, 10, "bad") == nullptr);
    CHECK(evg.begin(w, 1.f, 0.f, 0.f)); evg.end();
    CHECK(w->color[0] == 0xFFFF0000u);                         // cleared to opaque red

    // SDL2-style shim: quit
    SDL_Window* sw = SDL_CreateWindow("sdl", 0, 0, 100, 100, 0);
    CHECK(sw != nullptr);
    SDL_Event e; while (SDL_PollEvent(&e)) {}              // window creation queues resize events; drain them
    CHECK(SDL_PollEvent(&e) == 0);
    reinterpret_cast<evgx::Window*>(sw)->closed = true;        // simulate the user closing it
    CHECK(SDL_PollEvent(&e) == 1 && e.type == SDL_QUIT);
    CHECK(SDL_PollEvent(&e) == 0);                             // reported once
    SDL_DestroyWindow(sw);

    // ---- event queue, keyboard/mouse state through the SDL2 shim ----
    {
        evgx::Window* ew = evg.createwindow(50, 40, "events");
        CHECK(ew != nullptr);
        evgx::Event junk; while (evg.poll_event(junk)) {}      // drain leftovers
        ew->on_key('W', true);                                 // pressed
        ew->on_key('W', true, true);                           // repeat: still a KeyDown, repeat=true
        ew->on_key('W', false);
        evgx::Event ev;
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::KeyDown && ev.key == 'W' && !ev.repeat);
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::KeyDown && ev.repeat);
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::KeyUp && ev.key == 'W');
        CHECK(!evg.poll_event(ev));
        ew->on_mouse_move(10, 10);                             // first move: no delta event
        ew->on_mouse_move(13, 14);
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::MouseMotion && ev.dx == 3 && ev.dy == 4 && ev.x == 13);
        ew->on_button(0, true); ew->on_wheel(2.f); ew->on_button(0, false);
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::MouseDown && ev.button == 0 && ev.x == 13);
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::MouseWheel && ev.wheel == 2.f);
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::MouseUp);
        ew->on_resize(80, 60);
        CHECK(evg.poll_event(ev) && ev.type == evgx::EventType::Resize && ev.width == 80 && ew->width == 80);
        while (evg.poll_event(junk)) {}

        // SDL2 translation
        SDL_Event se;
        ew->on_key('A', true);
        CHECK(SDL_PollEvent(&se) == 1 && se.type == SDL_KEYDOWN);
        CHECK(se.key.keysym.sym == SDLK_a && se.key.keysym.scancode == SDL_SCANCODE_A && se.key.state == SDL_PRESSED);
        CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A] == 1);
        ew->on_key('A', false);
        CHECK(SDL_PollEvent(&se) == 1 && se.type == SDL_KEYUP && SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_A] == 0);
        ew->on_key(evgx::key::Right, true);
        CHECK(SDL_PollEvent(&se) == 1 && se.key.keysym.sym == SDLK_RIGHT && se.key.keysym.scancode == SDL_SCANCODE_RIGHT);
        CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RIGHT] == 1);
        ew->on_key(evgx::key::Right, false); while (SDL_PollEvent(&se)) {}
        CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_UP] == 0);
        CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_0] == 0);
        ew->on_key('0', true); CHECK(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_0] == 1); ew->on_key('0', false);
        while (SDL_PollEvent(&se)) {}

        ew->on_mouse_move(20, 21); while (SDL_PollEvent(&se)) {}
        SDL_GetRelativeMouseState(nullptr, nullptr);          // clear accumulated motion, as an app would each frame
        ew->on_button(2, true); ew->on_button(1, true);      // middle + right
        int mx = 0, my = 0;
        Uint32 mask = SDL_GetMouseState(&mx, &my);
        CHECK(mask == (SDL_BUTTON_MMASK | SDL_BUTTON_RMASK) && mx == 20 && my == 21);
        ew->on_mouse_move(25, 19);
        int rx = 0, ry = 0; SDL_GetRelativeMouseState(&rx, &ry);
        CHECK(rx == 5 && ry == -2);
        ew->on_button(2, false); ew->on_button(1, false);
        while (SDL_PollEvent(&se)) {}

        SDL_SetRelativeMouseMode(SDL_TRUE);
        CHECK(SDL_GetRelativeMouseMode() == SDL_TRUE);
        CHECK(SDL_GetTicks() < 1000000u);
        CHECK(SDL_GetPerformanceFrequency() > 0 && SDL_GetPerformanceCounter() > 0);
        int ww = 0, wh = 0; SDL_GetWindowSize(reinterpret_cast<SDL_Window*>(ew), &ww, &wh);
        CHECK(ww == 80 && wh == 60);
        evg.destroywindow(ew);
    }
    printf(fails ? "core: %d FAILED\n" : "core: all passed\n", fails);
    return fails != 0;
}
