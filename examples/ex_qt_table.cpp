// ex_qt_table: a table view filled from a small data set, then read back cell by cell. Closes after 3 seconds.
#include <stdio.h>
#include <evg/evg.hpp>

using evgx::QtWidget;

int main() {
    if (!evg.qt.init()) { printf("qt: %s\n", evg.error()); return 1; }
    QtWidget* win = evg.qt.createwindow("table demo", 420, 260);
    QtWidget* table = evg.qt.createtable(win, 3, 2, 10, 10, 400, 200);
    evg.qt.setheader(table, 0, "Item");
    evg.qt.setheader(table, 1, "Price");

    const char* items[3][2] = {{"Apple", "1.20"}, {"Bread", "2.50"}, {"Milk", "0.99"}};
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 2; c++) evg.qt.settablecell(table, r, c, items[r][c]);
    evg.qt.show(win);

    double t0 = evg.time();
    while (evg.time() - t0 < 3.0 && evg.qt.pump()) {}

    int ok = 0;
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 2; c++)
            if (evg.qt.gettablecell(table, r, c)[0] == items[r][c][0]) ok++;
    printf("cells read back: %d of 6\n", ok);
    evg.qt.close(win);
    return ok == 6 ? 0 : 1;
}
