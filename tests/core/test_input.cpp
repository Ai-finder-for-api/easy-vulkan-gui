// Input state on a window: held and pressed keys, mouse position and deltas, buttons, wheel, and frame timing.
#include "../common/check.hpp"

int main() {
    evgx::Window* w = evg.createwindow(64, 48, "input");
    CHECK(w != nullptr);
    if (!w) return evgtest::finish("core/input");

    // a key is held from press to release; pressed only on the frame it went down
    w->on_key('W', true);
    CHECK(w->key_down('W') && w->key_pressed('W'));
    w->open();                                   // a new frame: the edge flag clears, the held state stays
    CHECK(w->key_down('W') && !w->key_pressed('W'));
    w->on_key('W', true, true);                  // auto-repeat must not count as a new press
    CHECK(!w->key_pressed('W'));
    w->on_key('W', false);
    CHECK(!w->key_down('W'));

    // out-of-range key codes are ignored
    w->on_key(999, true);
    CHECK(!w->key_down(999));

    // the first mouse move only sets the reference point; later moves report a delta
    w->open();
    w->on_mouse_move(10, 10);
    CHECK(w->dx == 0.f && w->dy == 0.f);
    w->on_mouse_move(13, 6);
    CHECK(w->dx == 3.f && w->dy == -4.f && w->mouse_x == 13 && w->mouse_y == 6);

    // buttons: held state and range checks
    w->on_button(1, true);
    CHECK(w->button(1) && !w->button(0));
    w->on_button(1, false);
    CHECK(!w->button(1));
    w->on_button(7, true);                       // no such button
    CHECK(!w->button(7));

    // wheel accumulates over the frame and resets at the next open()
    w->open();
    w->on_wheel(1.5f);
    w->on_wheel(0.5f);
    CHECK(w->wheel == 2.f);
    w->open();
    CHECK(w->wheel == 0.f);

    // frame time is non-negative and clamped (long stalls do not explode the simulation)
    w->open();
    evg.sleep(5);
    w->open();
    CHECK(w->dt >= 0.f && w->dt <= 0.1f);

    evg.destroywindow(w);
    return evgtest::finish("core/input");
}
