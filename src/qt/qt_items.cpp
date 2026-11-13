// Item lists: combo boxes and list views. One set of functions works on both, since they share the same item model in spirit.
#include "qt_common.hpp"
#include <QByteArray>
#include <QComboBox>
#include <QListWidget>
#include <QString>

namespace evgx {
using namespace qtb;

namespace {
QByteArray g_item;   // storage behind getcurrenttext's returned pointer

QComboBox* as_combo(QtWidget* w) { return qobject_cast<QComboBox*>(W(w)); }
QListWidget* as_list(QtWidget* w) { return qobject_cast<QListWidget*>(W(w)); }
}

QtWidget* QtApi::createcombo(QtWidget* parent, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QComboBox(), parent, x, y, w, h);
}

QtWidget* QtApi::createlist(QtWidget* parent, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QListWidget(), parent, x, y, w, h);
}

void QtApi::additem(QtWidget* widget, const char* text) {
    QString s = QString::fromUtf8(text ? text : "");
    if (QComboBox* c = as_combo(widget)) c->addItem(s);
    else if (QListWidget* l = as_list(widget)) l->addItem(s);
}

void QtApi::clearitems(QtWidget* widget) {
    if (QComboBox* c = as_combo(widget)) c->clear();
    else if (QListWidget* l = as_list(widget)) l->clear();
}

void QtApi::setcurrentindex(QtWidget* widget, int index) {
    if (QComboBox* c = as_combo(widget)) c->setCurrentIndex(index);
    else if (QListWidget* l = as_list(widget)) l->setCurrentRow(index);
}

int QtApi::getcurrentindex(QtWidget* widget) {
    if (QComboBox* c = as_combo(widget)) return c->currentIndex();
    if (QListWidget* l = as_list(widget)) return l->currentRow();
    return -1;
}

const char* QtApi::getcurrenttext(QtWidget* widget) {
    QString s;
    if (QComboBox* c = as_combo(widget)) s = c->currentText();
    else if (QListWidget* l = as_list(widget)) { if (QListWidgetItem* it = l->currentItem()) s = it->text(); }
    g_item = s.toUtf8();
    return g_item.constData();
}

}  // namespace evgx
