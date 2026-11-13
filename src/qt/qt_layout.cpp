// Layouts: boxes that place their children (vertical or horizontal), window central widgets, spacing and margins.
#include "qt_common.hpp"
#include <QBoxLayout>
#include <QMainWindow>

namespace evgx {
using namespace qtb;

QtWidget* QtApi::createbox(QtWidget* parent, bool vertical, int x, int y, int w, int h) {
    if (!init()) return nullptr;
    QWidget* box = new QWidget();
    if (vertical) new QVBoxLayout(box);
    else new QHBoxLayout(box);
    return place(box, parent, x, y, w, h);
}

// adds a child to a box's layout; the layout decides its geometry from now on
void QtApi::addtobox(QtWidget* box, QtWidget* child) {
    QWidget* b = W(box);
    if (!b || !b->layout() || !child) return;
    b->layout()->addWidget(W(child));
    W(child)->show();
}

void QtApi::setcentral(QtWidget* window, QtWidget* widget) {
    if (QMainWindow* m = qobject_cast<QMainWindow*>(W(window))) m->setCentralWidget(W(widget));
}

void QtApi::setspacing(QtWidget* box, int px) {
    if (QBoxLayout* l = qobject_cast<QBoxLayout*>(W(box)->layout())) l->setSpacing(px);
}

void QtApi::setmargins(QtWidget* box, int px) {
    if (QBoxLayout* l = qobject_cast<QBoxLayout*>(W(box)->layout())) l->setContentsMargins(px, px, px, px);
}

}  // namespace evgx
