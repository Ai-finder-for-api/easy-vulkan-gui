// Custom painting: a widget whose paintEvent calls a user callback, and drawing calls that forward to QPainter.
// The painter handle is the QPainter that Qt set up for this paint; it is only valid inside the callback.
#include "qt_common.hpp"
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QPolygon>
#include <QString>
#include <QWidget>
#include <QPaintEvent>

namespace evgx {
using namespace qtb;

namespace {

QPainter* P(QtPainter* p) { return reinterpret_cast<QPainter*>(p); }

// a plain widget (no signals, so no moc) that forwards every paint to the user's callback
class PaintArea : public QWidget {
public:
    PaintArea(QtPaintFn fn, void* user) : fn_(fn), user_(user) {}
protected:
    void paintEvent(QPaintEvent*) override {
        if (!fn_) return;
        QPainter painter(this);
        painter.fillRect(rect(), palette().window());          // clear to the window color each time
        fn_(user_, reinterpret_cast<QtPainter*>(&painter), width(), height());
    }
private:
    QtPaintFn fn_;
    void* user_;
};

}  // namespace

QtWidget* QtApi::createpaint(QtWidget* parent, QtPaintFn fn, void* user, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new PaintArea(fn, user), parent, x, y, w, h);
}

void QtApi::requestpaint(QtWidget* widget) {
    if (widget) W(widget)->update();
}

void QtApi::setpaintcolor(QtPainter* p, int r, int g, int b, int a) {
    if (!p) return;
    QColor c(r, g, b, a);
    QPen pen = P(p)->pen();
    pen.setColor(c);
    P(p)->setPen(pen);
    P(p)->setBrush(c);
}

void QtApi::setpenwidth(QtPainter* p, float px) {
    if (!p) return;
    QPen pen = P(p)->pen();
    pen.setWidthF(px < 0.f ? 0.f : px);
    P(p)->setPen(pen);
}

void QtApi::setfontsize(QtPainter* p, int px) {
    if (!p || px <= 0) return;
    QFont f = P(p)->font();
    f.setPixelSize(px);
    P(p)->setFont(f);
}

void QtApi::setantialias(QtPainter* p, bool on) {
    if (!p) return;
    P(p)->setRenderHint(QPainter::Antialiasing, on);
}

void QtApi::fillrect(QtPainter* p, int x, int y, int w, int h) {
    if (p) P(p)->fillRect(x, y, w, h, P(p)->brush());
}

void QtApi::strokerect(QtPainter* p, int x, int y, int w, int h) {
    if (!p) return;
    P(p)->save();
    P(p)->setBrush(Qt::NoBrush);
    P(p)->drawRect(x, y, w, h);
    P(p)->restore();
}

void QtApi::drawline(QtPainter* p, int x1, int y1, int x2, int y2) {
    if (p) P(p)->drawLine(x1, y1, x2, y2);
}

void QtApi::fillellipse(QtPainter* p, int x, int y, int w, int h) {
    if (!p) return;
    P(p)->save();
    P(p)->setPen(Qt::NoPen);
    P(p)->drawEllipse(x, y, w, h);
    P(p)->restore();
}

void QtApi::strokeellipse(QtPainter* p, int x, int y, int w, int h) {
    if (!p) return;
    P(p)->save();
    P(p)->setBrush(Qt::NoBrush);
    P(p)->drawEllipse(x, y, w, h);
    P(p)->restore();
}

void QtApi::fillpolygon(QtPainter* p, const int* xy, int points) {
    if (!p || !xy || points < 3) return;
    QPolygon poly;
    for (int i = 0; i < points; i++) poly << QPoint(xy[2 * i], xy[2 * i + 1]);
    P(p)->drawPolygon(poly);
}

void QtApi::drawtext(QtPainter* p, int x, int y, const char* text) {
    if (p) P(p)->drawText(x, y, QString::fromUtf8(text ? text : ""));
}

}  // namespace evgx
