// viewer [model.glb]: orbit with left-drag + wheel; F toggles fly mode (WASD, QE, right-drag look, Shift = fast);
// P saves screenshot.bmp; Esc quits.
#include <evg/evg.hpp>

int main(int argc, char** argv) {
    using namespace evgx;
    Window* w = evg.createwindow(1280, 720, "evg viewer");
    if (!w) return 1;
    Model* m = evg.loadglb(argc > 1 ? argv[1] : "scene.glb");
    if (!m) return 2;                                   // evg.error() says why
    Orbit orbit; orbit.frame(m);
    Camera fly; bool flying = false;
    while (w->open()) {
        if (w->key_pressed(key::Esc)) break;
        if (w->key_pressed('F')) { flying = !flying; if (flying) fly = orbit.camera(); }
        if (w->key_pressed('P')) evg.screenshot(w, "screenshot.bmp");
        if (flying) fly.fly(*w, orbit.dist * 0.5f); else orbit.update(*w);
        if (!evg.begin(w, 0.10f, 0.12f, 0.18f)) { evg.sleep(16); continue; }   // minimized
        evg.camera(flying ? fly : orbit.camera());
        evg.draw(m);
        evg.end();
    }
    evg.freemodel(m);
    evg.destroywindow(w);
    return 0;
}
