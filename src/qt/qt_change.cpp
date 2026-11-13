// Change callbacks: check boxes and radio buttons (toggled), sliders and spin boxes (value changed), line and text edits,
// combo boxes and list views (selection changed), progress bars excluded (display only).
#include "qt_common.hpp"
#include <QAbstractButton>
#include <QAbstractSlider>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include <QSpinBox>
#include <QString>
#include <QTextEdit>

namespace evgx {
using namespace qtb;

void QtApi::setchange(QtWidget* widget, void (*fn)(void*), void* user) {
    QWidget* w = W(widget);
    if (QAbstractButton* b = qobject_cast<QAbstractButton*>(w))            // check boxes and radio buttons
        QObject::connect(b, &QAbstractButton::toggled, b, [fn, user](bool) { if (fn) fn(user); });
    else if (QAbstractSlider* s = qobject_cast<QAbstractSlider*>(w))
        QObject::connect(s, &QAbstractSlider::valueChanged, s, [fn, user](int) { if (fn) fn(user); });
    else if (QSpinBox* sp = qobject_cast<QSpinBox*>(w))
        QObject::connect(sp, &QSpinBox::valueChanged, sp, [fn, user](int) { if (fn) fn(user); });
    else if (QDoubleSpinBox* ds = qobject_cast<QDoubleSpinBox*>(w))
        QObject::connect(ds, &QDoubleSpinBox::valueChanged, ds, [fn, user](double) { if (fn) fn(user); });
    else if (QLineEdit* e = qobject_cast<QLineEdit*>(w))
        QObject::connect(e, &QLineEdit::textChanged, e, [fn, user](const QString&) { if (fn) fn(user); });
    else if (QTextEdit* t = qobject_cast<QTextEdit*>(w))
        QObject::connect(t, &QTextEdit::textChanged, t, [fn, user]() { if (fn) fn(user); });
    else if (QComboBox* c = qobject_cast<QComboBox*>(w))
        QObject::connect(c, &QComboBox::currentIndexChanged, c, [fn, user](int) { if (fn) fn(user); });
    else if (QListWidget* l = qobject_cast<QListWidget*>(w))
        QObject::connect(l, &QListWidget::currentRowChanged, l, [fn, user](int) { if (fn) fn(user); });
}

}  // namespace evgx
