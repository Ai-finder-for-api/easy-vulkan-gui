// Table views: creation, cell text get/set, header labels.
#include "qt_common.hpp"
#include <QByteArray>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace evgx {
using namespace qtb;

namespace {
QByteArray g_cell;   // storage behind gettablecell's returned pointer
}

QtWidget* QtApi::createtable(QtWidget* parent, int rows, int cols, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QTableWidget(rows, cols), parent, x, y, w, h);
}

void QtApi::settablecell(QtWidget* table, int row, int col, const char* text) {
    QTableWidget* t = qobject_cast<QTableWidget*>(W(table));
    if (!t) return;
    t->setItem(row, col, new QTableWidgetItem(QString::fromUtf8(text ? text : "")));
}

// "" for empty cells and invalid positions
const char* QtApi::gettablecell(QtWidget* table, int row, int col) {
    QTableWidget* t = qobject_cast<QTableWidget*>(W(table));
    QTableWidgetItem* it = t ? t->item(row, col) : nullptr;
    g_cell = it ? it->text().toUtf8() : QByteArray();
    return g_cell.constData();
}

void QtApi::setheader(QtWidget* table, int col, const char* text) {
    if (QTableWidget* t = qobject_cast<QTableWidget*>(W(table)))
        t->setHorizontalHeaderItem(col, new QTableWidgetItem(QString::fromUtf8(text ? text : "")));
}

}  // namespace evgx
