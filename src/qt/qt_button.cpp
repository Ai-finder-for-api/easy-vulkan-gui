// Push buttons: creation and click callbacks, and synthetic clicks.
#include "qt_common.hpp"
#include <QAbstractButton>
#include <QPushButton>
#include <QString>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::createbutton(QtWidget* parent, const char* text, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QPushButton(QString::fromUtf8(text ? text : "")), parent, x, y, w, h);
}

void QtApi::setclick(QtWidget* widget, void (*fn)(void*), void* user) {
    if (QAbstractButton* b = qobject_cast<QAbstractButton*>(W(widget)))
        QObject::connect(b, &QAbstractButton::clicked, b, [fn, user](bool) { if (fn) fn(user); });
}

// emits clicked() as if the user pressed it (disabled buttons ignore it)
void QtApi::click(QtWidget* widget) {
    if (QAbstractButton* b = qobject_cast<QAbstractButton*>(W(widget))) b->click();
}

}  // namespace evgx
