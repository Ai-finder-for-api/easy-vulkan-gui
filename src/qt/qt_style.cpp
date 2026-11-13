// Style sheets: applies Qt's CSS-like styling to any widget.
#include "qt_common.hpp"
#include <QString>

namespace evgx {
using namespace qtb;

void QtApi::setstylesheet(QtWidget* widget, const char* css) {
    if (widget) W(widget)->setStyleSheet(QString::fromUtf8(css ? css : ""));
}

}  // namespace evgx
