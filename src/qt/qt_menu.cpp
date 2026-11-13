// Menus and actions: menus on a window's menu bar, actions with optional shortcuts, separators, trigger by text.
#include "qt_common.hpp"
#include <QAction>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QString>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::addmenu(QtWidget* window, const char* title) {
    QMainWindow* m = qobject_cast<QMainWindow*>(W(window));
    if (!m) return nullptr;
    return H(m->menuBar()->addMenu(QString::fromUtf8(title ? title : "")));
}

bool QtApi::addaction(QtWidget* menu, const char* text, const char* shortcut, void (*fn)(void*), void* user) {
    QMenu* m = qobject_cast<QMenu*>(W(menu));
    if (!m) return false;
    QAction* a = m->addAction(QString::fromUtf8(text ? text : ""));
    if (shortcut && *shortcut) a->setShortcut(QKeySequence(QString::fromUtf8(shortcut)));
    QObject::connect(a, &QAction::triggered, a, [fn, user](bool) { if (fn) fn(user); });
    return true;
}

void QtApi::addseparator(QtWidget* menu) {
    if (QMenu* m = qobject_cast<QMenu*>(W(menu))) m->addSeparator();
}

// runs the first action whose text matches, ignoring '&' mnemonics
bool QtApi::triggeraction(QtWidget* menu, const char* text) {
    QMenu* m = qobject_cast<QMenu*>(W(menu));
    if (!m) return false;
    QString want = QString::fromUtf8(text ? text : "");
    want.remove(QLatin1Char('&'));
    for (QAction* a : m->actions()) {
        if (a->isSeparator()) continue;
        QString have = a->text();
        have.remove(QLatin1Char('&'));
        if (have == want) { a->trigger(); return true; }
    }
    return false;
}

}  // namespace evgx
