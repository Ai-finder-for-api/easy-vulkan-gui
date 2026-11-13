// Sliders, spin boxes and progress bars: creation, and the shared integer value get/set (setvalue/getvalue).
#include "qt_common.hpp"
#include <QAbstractSlider>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::createslider(QtWidget* parent, int min, int max, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    QSlider* s = new QSlider(Qt::Horizontal);
    s->setRange(min, max);
    return place(s, parent, x, y, w, h);
}

// sliders, integer spin boxes and progress bars all take an integer value
void QtApi::setvalue(QtWidget* widget, int value) {
    QWidget* w = W(widget);
    if (QAbstractSlider* s = qobject_cast<QAbstractSlider*>(w)) s->setValue(value);
    else if (QSpinBox* sp = qobject_cast<QSpinBox*>(w)) sp->setValue(value);
    else if (QProgressBar* p = qobject_cast<QProgressBar*>(w)) p->setValue(value);
}

int QtApi::getvalue(QtWidget* widget) {
    QWidget* w = W(widget);
    if (QAbstractSlider* s = qobject_cast<QAbstractSlider*>(w)) return s->value();
    if (QSpinBox* sp = qobject_cast<QSpinBox*>(w)) return sp->value();
    if (QProgressBar* p = qobject_cast<QProgressBar*>(w)) return p->value();
    return 0;
}

}  // namespace evgx
