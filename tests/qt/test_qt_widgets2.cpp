// Qt widgets added in this round: item lists, spin boxes, progress, radio buttons, text edits, containers, status bar,
// and change callbacks for each.
#include <string.h>
#include "../common/check.hpp"

using evgx::QtWidget;

static int g_changes = 0;
static void count(void*) { g_changes++; }

int main() {
    CHECK(evg.qt.init());
    QtWidget* win = evg.qt.createwindow("widgets2", 640, 480);
    CHECK(win != nullptr);
    if (!win) return evgtest::finish("qt/widgets2");

    // combo box: items, selection, text, change callback
    QtWidget* combo = evg.qt.createcombo(win, 10, 10, 150, 24);
    CHECK(combo != nullptr);
    evg.qt.additem(combo, "red");
    evg.qt.additem(combo, "green");
    evg.qt.additem(combo, "blue");
    evg.qt.setchange(combo, count, nullptr);
    g_changes = 0;
    evg.qt.setcurrentindex(combo, 1);
    CHECK(evg.qt.getcurrentindex(combo) == 1);
    CHECK(strcmp(evg.qt.getcurrenttext(combo), "green") == 0);
    CHECK(g_changes >= 1);
    evg.qt.clearitems(combo);
    CHECK(evg.qt.getcurrentindex(combo) == -1);
    CHECK(strcmp(evg.qt.getcurrenttext(combo), "") == 0);

    // list view: same API, single selection by row
    QtWidget* list = evg.qt.createlist(win, 10, 50, 150, 100);
    CHECK(list != nullptr);
    evg.qt.additem(list, "one");
    evg.qt.additem(list, "two");
    evg.qt.setcurrentindex(list, 0);
    CHECK(strcmp(evg.qt.getcurrenttext(list), "one") == 0);
    evg.qt.setcurrentindex(list, 1);
    CHECK(strcmp(evg.qt.getcurrenttext(list), "two") == 0);

    // integer spin box: value, range, change callback
    QtWidget* spin = evg.qt.createspinbox(win, 0, 100, 10, 160, 80, 24);
    CHECK(spin != nullptr);
    evg.qt.setchange(spin, count, nullptr);
    g_changes = 0;
    evg.qt.setvalue(spin, 42);
    CHECK(evg.qt.getvalue(spin) == 42);
    CHECK(g_changes >= 1);
    evg.qt.setvalue(spin, 500);                  // clamped to the range
    CHECK(evg.qt.getvalue(spin) == 100);

    // decimal spin box
    QtWidget* dspin = evg.qt.createdoublespinbox(win, -1.0, 1.0, 2, 10, 200, 80, 24);
    CHECK(dspin != nullptr);
    evg.qt.setdoublevalue(dspin, 0.25);
    CHECK(evg.qt.getdoublevalue(dspin) > 0.249 && evg.qt.getdoublevalue(dspin) < 0.251);

    // progress bar: value only, no user input
    QtWidget* prog = evg.qt.createprogress(win, 0, 10, 10, 240, 200, 20);
    CHECK(prog != nullptr);
    evg.qt.setvalue(prog, 7);
    CHECK(evg.qt.getvalue(prog) == 7);

    // radio buttons under one parent: checking one unchecks the other
    QtWidget* a = evg.qt.createradio(win, "A", 10, 270, 60, 20);
    QtWidget* b = evg.qt.createradio(win, "B", 80, 270, 60, 20);
    CHECK(a != nullptr && b != nullptr);
    evg.qt.setchecked(a, true);
    CHECK(evg.qt.getchecked(a) && !evg.qt.getchecked(b));
    evg.qt.setchecked(b, true);
    CHECK(!evg.qt.getchecked(a) && evg.qt.getchecked(b));

    // multi-line text edit: text round trip and read-only
    QtWidget* te = evg.qt.createtextedit(win, "line one\nline two", 10, 300, 300, 80);
    CHECK(te != nullptr);
    CHECK(strcmp(evg.qt.gettext(te), "line one\nline two") == 0);
    evg.qt.settext(te, "replaced");
    CHECK(strcmp(evg.qt.gettext(te), "replaced") == 0);
    evg.qt.setreadonly(te, true);
    evg.qt.setreadonly(te, false);

    // read-only line edit
    QtWidget* le = evg.qt.createlineedit(win, "fixed", 320, 300, 120, 24);
    CHECK(le != nullptr);
    evg.qt.setreadonly(le, true);
    CHECK(strcmp(evg.qt.gettext(le), "fixed") == 0);

    // group box: a titled frame; children created with it as parent
    QtWidget* group = evg.qt.creategroup(win, "Options", 10, 390, 250, 80);
    CHECK(group != nullptr);
    QtWidget* inside = evg.qt.createcheckbox(group, "inside", 10, 20, 100, 20);
    CHECK(inside != nullptr);
    evg.qt.setchecked(inside, true);
    CHECK(evg.qt.getchecked(inside));

    // tabs: each page takes children; switching pages keeps them
    QtWidget* tabs = evg.qt.createtabs(win, 280, 390, 340, 80);
    CHECK(tabs != nullptr);
    QtWidget* page1 = evg.qt.addtab(tabs, "First");
    QtWidget* page2 = evg.qt.addtab(tabs, "Second");
    CHECK(page1 != nullptr && page2 != nullptr && page1 != page2);
    QtWidget* btn = evg.qt.createbutton(page2, "in second", 5, 5, 100, 24);
    CHECK(btn != nullptr);

    // status bar of the window
    evg.qt.setstatus(win, "ready");

    // every new widget has to be reachable through the generic calls
    evg.qt.setenabled(combo, false);
    evg.qt.setenabled(combo, true);
    evg.qt.show(win);
    for (int i = 0; i < 3; i++) evg.qt.pump();
    CHECK(evg.qt.isvisible(combo));

    evg.qt.close(win);
    return evgtest::finish("qt/widgets2");
}
