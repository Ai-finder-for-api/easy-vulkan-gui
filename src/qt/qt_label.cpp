// Labels: creation. Text is read and written through the shared text functions (qt_text.cpp).
#include "qt_common.hpp"
#include <QLabel>
#include <QString>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::createlabel(QtWidget* parent, const char* text, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    return place(new QLabel(QString::fromUtf8(text ? text : "")), parent, x, y, w, h);
}

}  // namespace evgx
