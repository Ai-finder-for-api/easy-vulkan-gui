// Placement of Qt widgets: parent, geometry, and show-on-parent (a parentless widget stays hidden until shown or added to a box).
#include "qt_common.hpp"

namespace evgx { namespace qtb {

QtWidget* place(QWidget* w, QtWidget* parent, int x, int y, int ww, int hh) {
    if (parent) w->setParent(W(parent));
    w->setGeometry(x, y, ww, hh);
    if (parent) w->show();
    return H(w);
}

}}  // namespace evgx::qtb
