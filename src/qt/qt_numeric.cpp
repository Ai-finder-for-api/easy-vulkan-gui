// Numeric controls: spin boxes (integer and decimal) and progress bars. Integer values use setvalue/getvalue (qt_slider.cpp).
#include "qt_common.hpp"
#include <QDoubleSpinBox>
#include <QProgressBar>
#include <QSpinBox>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::createspinbox(QtWidget* parent, int min, int max, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    QSpinBox* s = new QSpinBox();
    s->setRange(min, max);
    return place(s, parent, x, y, w, h);
}

QtWidget* QtApi::createdoublespinbox(QtWidget* parent, double min, double max, int decimals, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    QDoubleSpinBox* s = new QDoubleSpinBox();
    s->setDecimals(decimals < 0 ? 0 : decimals);
    s->setRange(min, max);
    return place(s, parent, x, y, w, h);
}

void QtApi::setdoublevalue(QtWidget* widget, double value) {
    if (QDoubleSpinBox* s = qobject_cast<QDoubleSpinBox*>(W(widget))) s->setValue(value);
}

double QtApi::getdoublevalue(QtWidget* widget) {
    QDoubleSpinBox* s = qobject_cast<QDoubleSpinBox*>(W(widget));
    return s ? s->value() : 0.0;
}

QtWidget* QtApi::createprogress(QtWidget* parent, int min, int max, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    QProgressBar* p = new QProgressBar();
    p->setRange(min, max);
    return place(p, parent, x, y, w, h);
}

}  // namespace evgx
