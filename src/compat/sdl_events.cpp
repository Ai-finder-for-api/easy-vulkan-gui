// SDL2 event translation: evg's event queue entries become SDL_Event records (quit, keyboard, mouse, wheel, resize).
#include "sdl_internal.hpp"

namespace evgx { namespace sdl {
namespace {

Uint8 button_index(int b) {
    if (b == 0) return SDL_BUTTON_LEFT;
    if (b == 1) return SDL_BUTTON_RIGHT;
    return SDL_BUTTON_MIDDLE;
}

}

// fills `s` from an evg event; false for event kinds SDL has no equivalent of
bool translate(const Event& e, SDL_Event& s) {
    const Uint32 wid = 1;                                  // evg's windows are reported as window 1
    s = SDL_Event();
    switch (e.type) {
    case EventType::Quit:
        s.type = SDL_QUIT;
        return true;
    case EventType::KeyDown:
    case EventType::KeyUp:
        s.type = e.type == EventType::KeyDown ? SDL_KEYDOWN : SDL_KEYUP;
        s.key.windowID = wid;
        s.key.state = e.type == EventType::KeyDown ? SDL_PRESSED : SDL_RELEASED;
        s.key.repeat = e.repeat;
        s.key.keysym.scancode = scancode_of(e.key);
        s.key.keysym.sym = keycode_of(e.key);
        return true;
    case EventType::MouseMotion:
        s.type = SDL_MOUSEMOTION;
        s.motion.windowID = wid;
        s.motion.x = e.x;
        s.motion.y = e.y;
        s.motion.xrel = e.dx;
        s.motion.yrel = e.dy;
        return true;
    case EventType::MouseDown:
    case EventType::MouseUp:
        s.type = e.type == EventType::MouseDown ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
        s.button.windowID = wid;
        s.button.button = button_index(e.button);
        s.button.state = e.type == EventType::MouseDown ? SDL_PRESSED : SDL_RELEASED;
        s.button.clicks = 1;
        s.button.x = e.x;
        s.button.y = e.y;
        return true;
    case EventType::MouseWheel:
        s.type = SDL_MOUSEWHEEL;
        s.wheel.windowID = wid;
        s.wheel.y = (Sint32)e.wheel;
        s.wheel.x = 0;
        return true;
    case EventType::Resize:
        s.type = SDL_WINDOWEVENT;
        s.window.windowID = wid;
        s.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
        s.window.data1 = e.width;
        s.window.data2 = e.height;
        return true;
    default:
        return false;
    }
}

}}  // namespace evgx::sdl

extern "C" {
int SDL_PollEvent(SDL_Event* out) {
    evgx::Event e;
    if (!evg.poll_event(e)) return 0;
    SDL_Event s;
    if (!evgx::sdl::translate(e, s)) return 0;
    if (out) *out = s;
    return 1;
}
}
