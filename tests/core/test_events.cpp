// Event queue: ordering, field contents, quit reporting, and the overflow policy (new events dropped when full).
#include "../common/check.hpp"

int main() {
    evgx::Window* w = evg.createwindow(64, 48, "events");
    CHECK(w != nullptr);
    if (!w) return evgtest::finish("core/events");

    // drain anything queued by window creation
    evgx::Event e;
    while (evg.poll_event(e)) {}

    // ordering is first-in, first-out
    w->on_key('A', true);
    w->on_key('A', false);
    w->on_button(0, true);
    CHECK(evg.poll_event(e) && e.type == evgx::EventType::KeyDown && e.key == 'A');
    CHECK(evg.poll_event(e) && e.type == evgx::EventType::KeyUp && e.key == 'A');
    CHECK(evg.poll_event(e) && e.type == evgx::EventType::MouseDown && e.button == 0);
    CHECK(!evg.poll_event(e));

    // the window that produced the last event becomes the focus window
    CHECK(evg.focus_window() == w);

    // a resize updates the window and queues a Resize event carrying the new size
    w->on_resize(100, 70);
    CHECK(evg.poll_event(e) && e.type == evgx::EventType::Resize && e.width == 100 && e.height == 70);
    CHECK(w->width == 100 && w->height == 70);

    // overflow: the queue holds 256 events; later ones are dropped, the first 256 survive in order
    for (int i = 0; i < 300; i++) w->on_key(i % 26 + 'A', true);
    int got = 0, firstKey = -1;
    while (evg.poll_event(e)) {
        if (got == 0) firstKey = e.key;
        got++;
    }
    CHECK(got == 256);
    CHECK(firstKey == 'A');

    // closing a window produces exactly one Quit event
    w->closed = true;
    CHECK(evg.poll_event(e) && e.type == evgx::EventType::Quit);
    CHECK(!evg.poll_event(e));
    evg.destroywindow(w);

    return evgtest::finish("core/events");
}
