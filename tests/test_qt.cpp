// Qt integration: widgets through evg.qt.*, callbacks, and input forwarded from Qt events into an evg window.
#include <stdio.h>
#include <string.h>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QWidget>
#include <evg/evg.hpp>
#include <evg/qt/evg_qt.hpp>

using namespace evgx;


static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s   [%s]\n", __LINE__, #c, evg.error()); fails++; } } while (0)

static void bump(void* user) { ++*static_cast<int*>(user); }

struct AsyncState { int worked; int done; };
static void work_fn(void* u) { static_cast<AsyncState*>(u)->worked = 1; }
static void done_fn(void* u) { static_cast<AsyncState*>(u)->done += 1; }

int main(int argc, char** argv) {
    (void)argc; (void)argv;
    CHECK(evg.qt.available());
    CHECK(evg.qt.init());

    // ---- window, button, label, checkbox, slider, line edit ----
    QtWidget* win = evg.qt.createwindow("qt test", 400, 300);
    CHECK(win != nullptr);
    int clicks = 0, changes = 0;
    QtWidget* button = evg.qt.createbutton(win, "Press", 10, 10, 120, 30);
    CHECK(button != nullptr);
    evg.qt.setclick(button, bump, &clicks);
    evg.qt.click(button); evg.qt.click(button);
    CHECK(clicks == 2);
    evg.qt.settext(button, "Pressed");
    CHECK(evg.qt.gettext(button) && strcmp(evg.qt.gettext(button), "Pressed") == 0);

    QtWidget* label = evg.qt.createlabel(win, "label", 10, 50, 200, 20);
    evg.qt.settext(label, "hello label");
    CHECK(strcmp(evg.qt.gettext(label), "hello label") == 0);

    QtWidget* check = evg.qt.createcheckbox(win, "option", 10, 80, 150, 20);
    evg.qt.setchange(check, bump, &changes);
    evg.qt.setchecked(check, true);
    CHECK(evg.qt.getchecked(check) == true);
    CHECK(changes == 1);
    evg.qt.click(check);                                    // toggles back off, emits toggled
    CHECK(evg.qt.getchecked(check) == false && changes == 2);

    QtWidget* slider = evg.qt.createslider(win, 0, 100, 10, 110, 200, 20);
    evg.qt.setchange(slider, bump, &changes);
    evg.qt.setvalue(slider, 42);
    CHECK(evg.qt.getvalue(slider) == 42 && changes == 3);

    QtWidget* edit = evg.qt.createlineedit(win, "start", 10, 140, 200, 24);
    evg.qt.setchange(edit, bump, &changes);
    evg.qt.settext(edit, "typed");
    CHECK(strcmp(evg.qt.gettext(edit), "typed") == 0 && changes == 4);

    evg.qt.setenabled(button, false);
    evg.qt.click(button);                                   // disabled buttons do not click
    CHECK(clicks == 2);
    evg.qt.setenabled(button, true);

    evg.qt.setgeometry(label, 10, 170, 220, 20);
    evg.qt.show(win);
    CHECK(evg.qt.pump());                                   // a visible window keeps the loop alive

    // ---- canvas + input forwarding: Qt events -> evg window ----
    QtWidget* canvas = evg.qt.createcanvas(win, 10, 200, 380, 90);
    CHECK(canvas != nullptr);
    for (int i = 0; i < 40 && !evg.qt.canvas_window(canvas); i++) evg.qt.pump();
    Window* cw = evg.qt.canvas_window(canvas);
    CHECK(cw != nullptr);
    if (cw) {
        QWidget* cq = reinterpret_cast<QWidget*>(canvas);
        QKeyEvent down(QEvent::KeyPress, Qt::Key_W, Qt::NoModifier);
        QCoreApplication::sendEvent(cq, &down);
        CHECK(cw->key_down('W') && cw->key_pressed('W'));
        QKeyEvent up(QEvent::KeyRelease, Qt::Key_W, Qt::NoModifier);
        QCoreApplication::sendEvent(cq, &up);
        CHECK(!cw->key_down('W'));
        QKeyEvent arrow(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier);
        QCoreApplication::sendEvent(cq, &arrow);
        CHECK(cw->key_down(key::Left));
        QKeyEvent arrowUp(QEvent::KeyRelease, Qt::Key_Left, Qt::NoModifier);
        QCoreApplication::sendEvent(cq, &arrowUp);

        QMouseEvent press(QEvent::MouseButtonPress, QPointF(12, 34), QPointF(12, 34), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::sendEvent(cq, &press);
        CHECK(cw->button(0) && cw->mouse_x == 12 && cw->mouse_y == 34);
        QMouseEvent move(QEvent::MouseMove, QPointF(20, 40), QPointF(20, 40), Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
        QCoreApplication::sendEvent(cq, &move);
        CHECK(cw->mouse_x == 20 && cw->dx == 8.f && cw->dy == 6.f);
        QMouseEvent release(QEvent::MouseButtonRelease, QPointF(20, 40), QPointF(20, 40), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
        QCoreApplication::sendEvent(cq, &release);
        CHECK(!cw->button(0));

        // the events also land in the evg queue (what evg.poll_event returns)
        Event ev;
        int seen_key = 0;
        while (evg.poll_event(ev)) if (ev.type == EventType::KeyDown) seen_key++;
        CHECK(seen_key >= 2);

        // presentation keeps going while Qt runs
        int before = 0; for (int i = 0; i < 30; i++) { evg.qt.pump(); if (cw->closed) break; before++; }
        CHECK(before > 0);
        printf("canvas presented via Vulkan on: %s\n", evg.gpu());
    }
    // ---- layouts: children added to a box are placed and shown by the box ----
    QtWidget* box = evg.qt.createbox(nullptr, true, 0, 0, 0, 0);
    evg.qt.setcentral(win, box);
    QtWidget* la = evg.qt.createbutton(nullptr, "A", 0, 0, 50, 20);
    QtWidget* lb = evg.qt.createbutton(nullptr, "B", 0, 0, 50, 20);
    evg.qt.addtobox(box, la);
    evg.qt.addtobox(box, lb);
    evg.qt.setspacing(box, 6);
    evg.qt.setmargins(box, 4);
    for (int i = 0; i < 5; i++) evg.qt.pump();
    CHECK(evg.qt.isvisible(la) && evg.qt.isvisible(lb));

    // ---- menus and actions ----
    QtWidget* menu = evg.qt.addmenu(win, "File");
    CHECK(menu != nullptr);
    int saves = 0;
    CHECK(evg.qt.addaction(menu, "&Save", "Ctrl+S", bump, &saves));
    evg.qt.addseparator(menu);
    CHECK(evg.qt.addaction(menu, "Quit", nullptr, bump, &saves));
    CHECK(evg.qt.triggeraction(menu, "Save") && saves == 1);
    CHECK(evg.qt.triggeraction(menu, "Quit") && saves == 2);
    CHECK(!evg.qt.triggeraction(menu, "Missing"));

    // ---- timers ----
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

    // ---- settings ----
    evg.qt.setapp("evg-test", "qt-features");
    evg.qt.setsetting("theme", "dark");
    CHECK(strcmp(evg.qt.getsetting("theme", "light"), "dark") == 0);
    CHECK(strcmp(evg.qt.getsetting("no-such-key", "fallback"), "fallback") == 0);

    // ---- background work: worker thread, then GUI-thread completion ----
    AsyncState as{0, 0};
    CHECK(evg.qt.runasync(work_fn, &as, done_fn, &as));
    t0 = evg.time();
    while (as.done == 0 && evg.time() - t0 < 3.0) evg.qt.pump();
    CHECK(as.worked == 1 && as.done == 1);

    // ---- tables ----
    QtWidget* table = evg.qt.createtable(win, 2, 3, 10, 10, 300, 100);
    CHECK(table != nullptr);
    evg.qt.setheader(table, 0, "Name");
    evg.qt.settablecell(table, 1, 2, "cell");
    CHECK(strcmp(evg.qt.gettablecell(table, 1, 2), "cell") == 0);
    CHECK(strcmp(evg.qt.gettablecell(table, 0, 0), "") == 0);

    // ---- style sheets ----
    evg.qt.setstylesheet(button, "color: red;");

    // ---- dialogs: close them from a timer so the modal call returns ----
    QTimer::singleShot(100, [] { if (QWidget* m = QApplication::activeModalWidget()) m->close(); });
    CHECK(evg.qt.messagebox(win, "Info", "hello", 0) == 1);
    QTimer::singleShot(100, [] { if (QWidget* m = QApplication::activeModalWidget()) m->close(); });
    int answer = evg.qt.messagebox(win, "Question", "Continue?", 2);
    CHECK(answer == 0 || answer == 1);
    QTimer::singleShot(100, [] { if (QWidget* m = QApplication::activeModalWidget()) m->close(); });
    CHECK(strcmp(evg.qt.openfile(win, "Open", "All files (*)"), "") == 0);

    evg.qt.close(win);

    printf(fails ? "qt: %d FAILED\n" : "qt: all passed\n", fails);
    return fails != 0;
}
