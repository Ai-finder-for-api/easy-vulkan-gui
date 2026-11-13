// ex_qt_layout: a window built from a vertical box, a horizontal row inside it, and a menu. Closes after 4 seconds.
#include <stdio.h>
#include <evg/evg.hpp>

using evgx::QtWidget;

static int g_clicks = 0;
static void on_open(void*) { g_clicks++; }

int main() {
    if (!evg.qt.init()) { printf("qt: %s\n", evg.error()); return 1; }
    QtWidget* win = evg.qt.createwindow("layout demo", 480, 300);
    QtWidget* column = evg.qt.createbox(nullptr, true, 0, 0, 0, 0);
    QtWidget* row = evg.qt.createbox(nullptr, false, 0, 0, 0, 0);
    evg.qt.setcentral(win, column);
    evg.qt.addtobox(column, evg.qt.createlabel(nullptr, "Two buttons in a row:", 0, 0, 0, 0));
    evg.qt.addtobox(column, row);
    QtWidget* open = evg.qt.createbutton(nullptr, "Open", 0, 0, 0, 0);
    QtWidget* save = evg.qt.createbutton(nullptr, "Save", 0, 0, 0, 0);
    evg.qt.setclick(open, on_open, nullptr);
    evg.qt.addtobox(row, open);
    evg.qt.addtobox(row, save);
    evg.qt.setspacing(row, 8);
    evg.qt.setmargins(column, 12);

    QtWidget* file = evg.qt.addmenu(win, "&File");
    evg.qt.addaction(file, "&Open", "Ctrl+O", on_open, nullptr);
    evg.qt.addaction(file, "&Save", "Ctrl+S", nullptr, nullptr);
    evg.qt.addseparator(file);
    evg.qt.addaction(file, "E&xit", "Ctrl+Q", nullptr, nullptr);

    evg.qt.show(win);
    double t0 = evg.time();
    evg.qt.click(open);                              // a scripted click; the window is otherwise idle
    while (evg.time() - t0 < 4.0 && evg.qt.pump()) {}
    evg.qt.triggeraction(file, "Open");              // a menu action, also after the loop
    printf("button and menu actions: %d\n", g_clicks);
    return g_clicks == 2 ? 0 : 1;
}
