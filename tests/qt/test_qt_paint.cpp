// Custom painting: the paint callback runs on real paint events, receives the widget size, draws with the evg painter
// functions, and is called again when a repaint is requested. Pixels are checked through QWidget::grab().
#include <QColor>
#include <QImage>
#include <QWidget>
#include "../common/check.hpp"

using evgx::QtPainter;
using evgx::QtWidget;

struct PaintState {
    int calls = 0;
    int lastW = 0, lastH = 0;
};

static void paint(void* user, QtPainter* p, int w, int h) {
    PaintState* s = static_cast<PaintState*>(user);
    s->calls++;
    s->lastW = w;
    s->lastH = h;
    evg.qt.setantialias(p, false);
    evg.qt.setpaintcolor(p, 255, 0, 0);                 // red left half
    evg.qt.fillrect(p, 0, 0, w / 2, h);
    evg.qt.setpaintcolor(p, 0, 0, 255);                 // blue ellipse on the right
    evg.qt.fillellipse(p, w / 2, 0, w / 2, h);
    int tri[] = {0, 0, 10, 0, 0, 10};
    evg.qt.setpaintcolor(p, 0, 255, 0);
    evg.qt.fillpolygon(p, tri, 3);
    evg.qt.setpenwidth(p, 2.0f);
    evg.qt.setpaintcolor(p, 255, 255, 255);
    evg.qt.drawline(p, 0, 0, w, h);
    evg.qt.setfontsize(p, 12);
    evg.qt.drawtext(p, 5, h - 5, "evg");
    evg.qt.strokerect(p, 1, 1, w - 2, h - 2);
}

int main() {
    CHECK(evg.qt.init());
    QtWidget* win = evg.qt.createwindow("paint", 200, 100);
    CHECK(win != nullptr);
    if (!win) return evgtest::finish("qt/paint");

    PaintState state;
    QtWidget* area = evg.qt.createpaint(win, paint, &state, 0, 0, 200, 100);
    CHECK(area != nullptr);
    if (!area) return evgtest::finish("qt/paint");
    evg.qt.show(win);
    for (int i = 0; i < 5; i++) evg.qt.pump();

    // grab() performs a real paint of the widget; the callback has to have run
    QImage img = reinterpret_cast<QWidget*>(area)->grab().toImage();
    CHECK(state.calls >= 1);
    CHECK(state.lastW == 200 && state.lastH == 100);
    // the grab is at device resolution (250x125 at 125% scaling); compare logical sizes and map logical pixels
    const double dpr = img.devicePixelRatio();
    CHECK((int)(img.width() / dpr + 0.5) == 200 && (int)(img.height() / dpr + 0.5) == 100);
    auto at = [&](int x, int y) { return img.pixelColor((int)(x * dpr), (int)(y * dpr)); };
    QColor left = at(30, 50);                            // inside the red rectangle, away from the line
    CHECK(left.red() == 255 && left.green() == 0 && left.blue() == 0);
    QColor triangle = at(3, 4);                          // inside the green triangle, clear of the border and the diagonal
    CHECK(triangle.green() == 255 && triangle.red() == 0);
    QColor right = at(170, 20);                          // inside the blue ellipse
    CHECK(right.blue() == 255 && right.red() == 0);

    // a repaint request runs the callback again
    int before = state.calls;
    evg.qt.requestpaint(area);
    for (int i = 0; i < 5; i++) evg.qt.pump();
    CHECK(state.calls > before);

    // painter calls outside a callback are ignored, not crashes
    evg.qt.setpaintcolor(nullptr, 1, 2, 3);
    evg.qt.fillrect(nullptr, 0, 0, 10, 10);
    evg.qt.drawtext(nullptr, 0, 0, "x");
    int tri[] = {0, 0, 1, 1};                            // fewer than three points: ignored
    evg.qt.fillpolygon(nullptr, tri, 2);

    evg.qt.close(win);
    return evgtest::finish("qt/paint");
}
