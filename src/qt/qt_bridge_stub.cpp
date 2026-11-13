// evg.qt.* without Qt: every call fails with a clear message, and every function the API declares has a definition
// so the library links. The real bridge is src/qt/*.cpp and is compiled when EVG_WITH_QT=ON.
#include <evg/evg.hpp>

namespace evgx {
namespace {
const char* kNoQt = "evg was built without Qt: configure with -DEVG_WITH_QT=ON and Qt6 Widgets";
}
bool QtApi::available() const { return false; }
bool QtApi::init() { evg.seterror(kNoQt); return false; }
bool QtApi::pump() { return false; }
void QtApi::quit() {}
QtWidget* QtApi::createwindow(const char*, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createbutton(QtWidget*, const char*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createlabel(QtWidget*, const char*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createcheckbox(QtWidget*, const char*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createslider(QtWidget*, int, int, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createlineedit(QtWidget*, const char*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createcanvas(QtWidget*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
Window* QtApi::canvas_window(QtWidget*) { return nullptr; }
void QtApi::setclick(QtWidget*, void (*)(void*), void*) {}
void QtApi::setchange(QtWidget*, void (*)(void*), void*) {}
void QtApi::settext(QtWidget*, const char*) {}
const char* QtApi::gettext(QtWidget*) { return ""; }
void QtApi::setvalue(QtWidget*, int) {}
int  QtApi::getvalue(QtWidget*) { return 0; }
void QtApi::setchecked(QtWidget*, bool) {}
bool QtApi::getchecked(QtWidget*) { return false; }
void QtApi::setenabled(QtWidget*, bool) {}
void QtApi::setgeometry(QtWidget*, int, int, int, int) {}
void QtApi::show(QtWidget*) {}
void QtApi::hide(QtWidget*) {}
void QtApi::click(QtWidget*) {}
void QtApi::close(QtWidget*) {}
bool QtApi::isvisible(QtWidget*) { return false; }

QtWidget* QtApi::createbox(QtWidget*, bool, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
void QtApi::addtobox(QtWidget*, QtWidget*) {}
void QtApi::setcentral(QtWidget*, QtWidget*) {}
void QtApi::setspacing(QtWidget*, int) {}
void QtApi::setmargins(QtWidget*, int) {}

QtWidget* QtApi::addmenu(QtWidget*, const char*) { evg.seterror(kNoQt); return nullptr; }
bool QtApi::addaction(QtWidget*, const char*, const char*, void (*)(void*), void*) { evg.seterror(kNoQt); return false; }
void QtApi::addseparator(QtWidget*) {}
bool QtApi::triggeraction(QtWidget*, const char*) { return false; }

int QtApi::messagebox(QtWidget*, const char*, const char*, int) { evg.seterror(kNoQt); return -1; }
const char* QtApi::openfile(QtWidget*, const char*, const char*) { evg.seterror(kNoQt); return ""; }
const char* QtApi::savefile(QtWidget*, const char*, const char*) { evg.seterror(kNoQt); return ""; }

int  QtApi::createtimer(int, void (*)(void*), void*) { evg.seterror(kNoQt); return 0; }
void QtApi::stoptimer(int) {}

void QtApi::setapp(const char*, const char*) {}
void QtApi::setsetting(const char*, const char*) {}
const char* QtApi::getsetting(const char*, const char* fallback) { return fallback ? fallback : ""; }

bool QtApi::runasync(void (*)(void*), void*, void (*)(void*), void*) { evg.seterror(kNoQt); return false; }

QtWidget* QtApi::createtable(QtWidget*, int, int, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
void QtApi::settablecell(QtWidget*, int, int, const char*) {}
const char* QtApi::gettablecell(QtWidget*, int, int) { return ""; }
void QtApi::setheader(QtWidget*, int, const char*) {}

void QtApi::setstylesheet(QtWidget*, const char*) {}

QtWidget* QtApi::createradio(QtWidget*, const char*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createspinbox(QtWidget*, int, int, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createdoublespinbox(QtWidget*, double, double, int, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
void QtApi::setdoublevalue(QtWidget*, double) {}
double QtApi::getdoublevalue(QtWidget*) { return 0.0; }
QtWidget* QtApi::createprogress(QtWidget*, int, int, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createtextedit(QtWidget*, const char*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
void QtApi::setreadonly(QtWidget*, bool) {}

QtWidget* QtApi::createcombo(QtWidget*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createlist(QtWidget*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
void QtApi::additem(QtWidget*, const char*) {}
void QtApi::clearitems(QtWidget*) {}
void QtApi::setcurrentindex(QtWidget*, int) {}
int  QtApi::getcurrentindex(QtWidget*) { return -1; }
const char* QtApi::getcurrenttext(QtWidget*) { return ""; }

QtWidget* QtApi::creategroup(QtWidget*, const char*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::createtabs(QtWidget*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
QtWidget* QtApi::addtab(QtWidget*, const char*) { evg.seterror(kNoQt); return nullptr; }
void QtApi::setstatus(QtWidget*, const char*) {}

QtWidget* QtApi::createpaint(QtWidget*, QtPaintFn, void*, int, int, int, int) { evg.seterror(kNoQt); return nullptr; }
void QtApi::requestpaint(QtWidget*) {}
void QtApi::setpaintcolor(QtPainter*, int, int, int, int) {}
void QtApi::setpenwidth(QtPainter*, float) {}
void QtApi::setfontsize(QtPainter*, int) {}
void QtApi::setantialias(QtPainter*, bool) {}
void QtApi::fillrect(QtPainter*, int, int, int, int) {}
void QtApi::strokerect(QtPainter*, int, int, int, int) {}
void QtApi::drawline(QtPainter*, int, int, int, int) {}
void QtApi::fillellipse(QtPainter*, int, int, int, int) {}
void QtApi::strokeellipse(QtPainter*, int, int, int, int) {}
void QtApi::fillpolygon(QtPainter*, const int*, int) {}
void QtApi::drawtext(QtPainter*, int, int, const char*) {}

}  // namespace evgx
