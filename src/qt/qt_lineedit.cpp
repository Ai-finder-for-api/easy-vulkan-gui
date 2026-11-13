// Single-line text edits: creation. Text get/set goes through qt_text.cpp.
#include "qt_common.hpp"
#include <QLineEdit>
#include <QString>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::createlineedit(QtWidget* parent, const char* text, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QLineEdit(QString::fromUtf8(text ? text : "")), parent, x, y, w, h);
}

}  // namespace evgx
