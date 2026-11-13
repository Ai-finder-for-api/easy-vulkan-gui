// Qt features: layouts, menus, timers, settings, background work, tables, style sheets, and modal dialogs (closed from a timer).
#include <string.h>
#include <QApplication>
#include <QTimer>
#include <QWidget>
#include "../common/check.hpp"

using evgx::QtWidget;

static void bump(void* user) { ++*static_cast<int*>(user); }
struct Async { int worked; int done; };
static void work(void* u) { static_cast<Async*>(u)->worked = 1; }
static void finished(void* u) { static_cast<Async*>(u)->done += 1; }

int main() {
    CHECK(evg.qt.init());
    QtWidget* win = evg.qt.createwindow("features", 480, 360);
    CHECK(win != nullptr);
    if (!win) return evgtest::finish("qt/features");

    // layouts: children added to a box are placed and shown by it
    QtWidget* box = evg.qt.createbox(nullptr, true, 0, 0, 0, 0);
    evg.qt.setcentral(win, box);
    QtWidget* a = evg.qt.createbutton(nullptr, "A", 0, 0, 50, 20);
    QtWidget* b = evg.qt.createbutton(nullptr, "B", 0, 0, 50, 20);
    evg.qt.addtobox(box, a);
    evg.qt.addtobox(box, b);
    evg.qt.setspacing(box, 6);
    evg.qt.setmargins(box, 4);
    evg.qt.show(win);
    for (int i = 0; i < 5; i++) evg.qt.pump();
    CHECK(evg.qt.isvisible(a) && evg.qt.isvisible(b));

    // menus and actions: triggered by text, shortcut accepted, '&' ignored when matching
    QtWidget* menu = evg.qt.addmenu(win, "File");
    CHECK(menu != nullptr);
    int saves = 0;
    CHECK(evg.qt.addaction(menu, "&Save", "Ctrl+S", bump, &saves));
    evg.qt.addseparator(menu);
    CHECK(evg.qt.addaction(menu, "Quit", nullptr, bump, &saves));
    CHECK(evg.qt.triggeraction(menu, "Save") && saves == 1);
    CHECK(evg.qt.triggeraction(menu, "Quit") && saves == 2);
    CHECK(!evg.qt.triggeraction(menu, "Missing"));

    // timers: fire while the loop runs, stop cleanly
    int ticks = 0;
    int tid = evg.qt.createtimer(5, bump, &ticks);
    CHECK(tid > 0);
    double t0 = evg.time();
    while (evg.time() - t0 < 0.2) evg.qt.pump();
    CHECK(ticks >= 5);
    evg.qt.stoptimer(tid);
    for (int i = 0; i < 5; i++) evg.qt.pump();
    int frozen = ticks;
    t0 = evg.time();
    while (evg.time() - t0 < 0.05) evg.qt.pump();
    CHECK(ticks == frozen);

    // settings: round trip and fallback
    evg.qt.setapp("evg-test", "qt-features");
    evg.qt.setsetting("theme", "dark");
    CHECK(strcmp(evg.qt.getsetting("theme", "light"), "dark") == 0);
    CHECK(strcmp(evg.qt.getsetting("no-such-key", "fallback"), "fallback") == 0);

    // background work: runs on a worker thread, completion on the GUI thread
    Async as = {0, 0};
    CHECK(evg.qt.runasync(work, &as, finished, &as));
    t0 = evg.time();
    while (as.done == 0 && evg.time() - t0 < 3.0) evg.qt.pump();
    CHECK(as.worked == 1 && as.done == 1);

    // tables
    QtWidget* table = evg.qt.createtable(win, 2, 3, 10, 10, 300, 100);
    CHECK(table != nullptr);
    evg.qt.setheader(table, 0, "Name");
    evg.qt.settablecell(table, 1, 2, "cell");
    CHECK(strcmp(evg.qt.gettablecell(table, 1, 2), "cell") == 0);
    CHECK(strcmp(evg.qt.gettablecell(table, 0, 0), "") == 0);

    // style sheets apply without error
    evg.qt.setstylesheet(a, "color: red;");

    // dialogs: closed from a timer so the modal call returns
    QTimer::singleShot(100, [] { if (QWidget* m = QApplication::activeModalWidget()) m->close(); });
    CHECK(evg.qt.messagebox(win, "Info", "hello", 0) == 1);
    QTimer::singleShot(100, [] { if (QWidget* m = QApplication::activeModalWidget()) m->close(); });
    int answer = evg.qt.messagebox(win, "Question", "Continue?", 2);
    CHECK(answer == 0 || answer == 1);
    QTimer::singleShot(100, [] { if (QWidget* m = QApplication::activeModalWidget()) m->close(); });
    CHECK(strcmp(evg.qt.openfile(win, "Open", "All files (*)"), "") == 0);

    evg.qt.close(win);
    return evgtest::finish("qt/features");
}
