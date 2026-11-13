// Geometry and visibility of any widget: enable state, position and size, show/hide/close, visibility query.
#include "qt_common.hpp"

namespace evgx {
using namespace qtb;

void QtApi::setenabled(QtWidget* widget, bool on) { W(widget)->setEnabled(on); }
void QtApi::setgeometry(QtWidget* widget, int x, int y, int w, int h) { W(widget)->setGeometry(x, y, w, h); }
void QtApi::show(QtWidget* widget) { W(widget)->show(); }
void QtApi::hide(QtWidget* widget) { W(widget)->hide(); }
void QtApi::close(QtWidget* widget) { W(widget)->close(); }
bool QtApi::isvisible(QtWidget* widget) { return W(widget)->isVisible(); }

}  // namespace evgx
