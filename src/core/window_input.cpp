// Per-window input state: Window::open() (frame boundary), key, mouse, wheel and resize injection.
#include "state.hpp"
#include "platform.hpp"

namespace evgx {
using namespace st;

bool Window::open() {
    for (int k = 0; k < 256; k++) pressed[k] = 0;   // "pressed this frame" lasts one frame
    dx = dy = wheel = 0;
    plat::pump();
    double now = plat::time_seconds();
    dt = last_time < 0 ? 0.f : (float)(now - last_time);
    if (dt > 0.1f) dt = 0.1f;                       // clamp long stalls (window dragging, breakpoints)
    last_time = now;
    return !closed;
}

void Window::on_key(int k, bool down, bool repeat) {
    if (k < 0 || k > 255) return;
    if (down && !repeat && !keys[k]) pressed[k] = 1;
    keys[k] = down ? 1 : 0;
    Event e;
    e.type = down ? EventType::KeyDown : EventType::KeyUp;
    e.window = this; e.key = k; e.repeat = repeat;
    push_event(e);
}

void Window::on_mouse_move(int x, int y) {
    int ddx = 0, ddy = 0;
    if (had_mouse) { ddx = x - mouse_x; ddy = y - mouse_y; dx += (float)ddx; dy += (float)ddy; }
    mouse_x = x; mouse_y = y;
    if (had_mouse) {
        Event e;
        e.type = EventType::MouseMotion;
        e.window = this; e.x = x; e.y = y; e.dx = ddx; e.dy = ddy;
        push_event(e);
    }
    had_mouse = true;
}

void Window::on_button(int b, bool down) {
    if (b < 0 || b > 2) return;
    buttons[b] = down;
    Event e;
    e.type = down ? EventType::MouseDown : EventType::MouseUp;
    e.window = this; e.button = b; e.x = mouse_x; e.y = mouse_y;
    push_event(e);
}

void Window::on_wheel(float notches) {
    wheel += notches;
    Event e;
    e.type = EventType::MouseWheel;
    e.window = this; e.wheel = notches; e.x = mouse_x; e.y = mouse_y;
    push_event(e);
}

void Window::on_resize(int w, int h) {
    width = w; height = h;
    Event e;
    e.type = EventType::Resize;
    e.window = this; e.width = w; e.height = h;
    push_event(e);
}

}  // namespace evgx
