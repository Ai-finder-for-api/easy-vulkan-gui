#include <evg/evg.hpp>
#include <evg/compat/glm.hpp>

int main() {
    evgx::Window* w = evg.createwindow(1280, 720, "hello evg");
    if (!w) return 1;                       // evg.error() says why
    while (w->open()) {
        if (evg.begin(w, 0.1f, 0.2f, 0.4f)) evg.end();
        evg.sleep(16);
    }
    evg.destroywindow(w);
    return 0;
}
