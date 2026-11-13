// Qt application lifetime and top-level windows: init, pump, quit, createwindow.
#include "qt_common.hpp"
#include <QApplication>
#include <QEventLoop>
#include <QMainWindow>
#include <QString>

namespace evgx {
using namespace qtb;

bool QtApi::available() const { return true; }

// creates the QApplication once, with a fixed argv (evg is not a command-line tool)
bool QtApi::init() {
    if (qApp) return true;
    static int argc = 1;
    static char name[] = "evg";
    static char* argv[] = {name, nullptr};
    new QApplication(argc, argv);
    return qApp != nullptr;
}

// processes pending events briefly; false once no top-level Qt window is visible any more
bool QtApi::pump() {
    if (!qApp) return false;
    qApp->processEvents(QEventLoop::AllEvents, 2);
    for (QWidget* top : QApplication::topLevelWidgets())
        if (top->isVisible()) return true;
    return false;
}

void QtApi::quit() {
    for (QWidget* top : QApplication::topLevelWidgets()) top->close();
}

// a main window (menu bar and central widget); children are placed in its client area
QtWidget* QtApi::createwindow(const char* title, int w, int h) {
    if (!init()) return nullptr;
    QMainWindow* win = new QMainWindow();
    win->setWindowTitle(QString::fromUtf8(title ? title : "evg"));
    win->resize(w, h);
    return H(win);
}

}  // namespace evgx
