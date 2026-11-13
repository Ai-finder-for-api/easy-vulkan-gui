// Text of any widget that has one: buttons, labels, line edits, multi-line text edits, and window titles for everything else.
#include "qt_common.hpp"
#include <QAbstractButton>
#include <QByteArray>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QTextEdit>

namespace evgx {
using namespace qtb;

namespace {
QByteArray g_text;   // storage behind gettext's returned pointer, valid until the next call
}

QtWidget* QtApi::createtextedit(QtWidget* parent, const char* text, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    QTextEdit* e = new QTextEdit();
    e->setPlainText(QString::fromUtf8(text ? text : ""));
    return place(e, parent, x, y, w, h);
}

void QtApi::setreadonly(QtWidget* widget, bool on) {
    QWidget* w = W(widget);
    if (QLineEdit* e = qobject_cast<QLineEdit*>(w)) e->setReadOnly(on);
    else if (QTextEdit* t = qobject_cast<QTextEdit*>(w)) t->setReadOnly(on);
}

void QtApi::settext(QtWidget* widget, const char* text) {
    QWidget* w = W(widget);
    QString s = QString::fromUtf8(text ? text : "");
    if (QAbstractButton* b = qobject_cast<QAbstractButton*>(w)) b->setText(s);
    else if (QLabel* l = qobject_cast<QLabel*>(w)) l->setText(s);
    else if (QLineEdit* e = qobject_cast<QLineEdit*>(w)) e->setText(s);
    else if (QTextEdit* t = qobject_cast<QTextEdit*>(w)) t->setPlainText(s);
    else w->setWindowTitle(s);
}

const char* QtApi::gettext(QtWidget* widget) {
    QWidget* w = W(widget);
    QString s;
    if (QAbstractButton* b = qobject_cast<QAbstractButton*>(w)) s = b->text();
    else if (QLabel* l = qobject_cast<QLabel*>(w)) s = l->text();
    else if (QLineEdit* e = qobject_cast<QLineEdit*>(w)) s = e->text();
    else if (QTextEdit* t = qobject_cast<QTextEdit*>(w)) s = t->toPlainText();
    else s = w->windowTitle();
    g_text = s.toUtf8();
    return g_text.constData();
}

}  // namespace evgx
