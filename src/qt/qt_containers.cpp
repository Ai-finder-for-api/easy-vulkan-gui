// Containers: titled group boxes, tab sets with page widgets, and the status bar of a window.
#include "qt_common.hpp"
#include <QGroupBox>
#include <QMainWindow>
#include <QStatusBar>
#include <QString>
#include <QTabWidget>
#include <QWidget>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::creategroup(QtWidget* parent, const char* title, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    QGroupBox* g = new QGroupBox(QString::fromUtf8(title ? title : ""));
    return place(g, parent, x, y, w, h);
}

QtWidget* QtApi::createtabs(QtWidget* parent, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QTabWidget(), parent, x, y, w, h);
}

// adds a page to a tab set; the page is a plain widget that holds the controls of that tab
QtWidget* QtApi::addtab(QtWidget* tabs, const char* title) {
    QTabWidget* t = qobject_cast<QTabWidget*>(W(tabs));
    if (!t) return nullptr;
    QWidget* page = new QWidget();
    t->addTab(page, QString::fromUtf8(title ? title : ""));
    return H(page);
}

void QtApi::setstatus(QtWidget* window, const char* text) {
    if (QMainWindow* m = qobject_cast<QMainWindow*>(W(window)))
        m->statusBar()->showMessage(QString::fromUtf8(text ? text : ""));
}

}  // namespace evgx
