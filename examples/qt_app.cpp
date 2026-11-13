// qt_app: an evg.qt application: a window with a label, a button, a slider and a Vulkan canvas.
// Everything is built with evg.qt.* calls. Close the window to exit.
#include <stdio.h>
#include <evg/evg.hpp>

using namespace evgx;

struct App { QtWidget* label = nullptr; QtWidget* slider = nullptr; int clicks = 0; };

static void on_click(void* user) {
    App* a = static_cast<App*>(user);
    char text[64];
    snprintf(text, sizeof text, "button clicked %d time%s", ++a->clicks, a->clicks == 1 ? "" : "s");
    evg.qt.settext(a->label, text);
}

static void on_slide(void* user) {
    App* a = static_cast<App*>(user);
    char text[64];
    snprintf(text, sizeof text, "slider at %d", evg.qt.getvalue(a->slider));
    evg.qt.settext(a->label, text);
}

int main() {
    if (!evg.qt.init()) { printf("qt: %s\n", evg.error()); return 1; }

    QtWidget* win = evg.qt.createwindow("evg qt app", 480, 420);
    App app;
    app.label = evg.qt.createlabel(win, "ready", 10, 10, 300, 20);

    QtWidget* button = evg.qt.createbutton(win, "Click me", 10, 40, 140, 32);
    evg.qt.setclick(button, on_click, &app);

    app.slider = evg.qt.createslider(win, 0, 100, 10, 90, 300, 24);
    evg.qt.setchange(app.slider, on_slide, &app);

    QtWidget* check = evg.qt.createcheckbox(win, "show the canvas", 10, 130, 200, 24);
    evg.qt.setchecked(check, true);

    QtWidget* canvas = evg.qt.createcanvas(win, 10, 170, 460, 230);
    evg.qt.show(win);

    int frames = 0;
    double t0 = evg.time();
    while (evg.qt.pump()) {
        if (evg.qt.getchecked(check)) evg.qt.show(canvas); else evg.qt.hide(canvas);
        Window* cw = evg.qt.canvas_window(canvas);
        if (cw && !cw->closed) frames++;
        Event e;
        while (evg.poll_event(e)) if (e.type == EventType::KeyDown) printf("key %d\n", e.key);
    }
    double secs = evg.time() - t0;
    printf("gpu: %s\n", evg.gpu());
    printf("clicks: %d, canvas frames: %d in %.1f s\n", app.clicks, frames, secs);
    return 0;
}
