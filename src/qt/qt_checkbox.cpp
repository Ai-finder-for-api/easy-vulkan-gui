// Check boxes and radio buttons: creation and checked state.
#include "qt_common.hpp"
#include <QAbstractButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QString>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::createcheckbox(QtWidget* parent, const char* text, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QCheckBox(QString::fromUtf8(text ? text : "")), parent, x, y, w, h);
}

// radio buttons with the same parent form one exclusive group
QtWidget* QtApi::createradio(QtWidget* parent, const char* text, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QRadioButton(QString::fromUtf8(text ? text : "")), parent, x, y, w, h);
}

void QtApi::setchecked(QtWidget* widget, bool on) {
    if (QAbstractButton* b = qobject_cast<QAbstractButton*>(W(widget))) b->setChecked(on);
}

bool QtApi::getchecked(QtWidget* widget) {
    QAbstractButton* b = qobject_cast<QAbstractButton*>(W(widget));
    return b ? b->isChecked() : false;
}

}  // namespace evgx
