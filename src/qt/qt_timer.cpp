// Repeating timers: create with an interval and callback, stop by id. Ids are 1-based and never reused.
#include "qt_common.hpp"
#include <QApplication>
#include <QTimer>

namespace evgx {

namespace {
constexpr int kTimers = 64;
QTimer* g_timers[kTimers];
int g_ntimers = 0;
}

int QtApi::createtimer(int ms, void (*fn)(void*), void* user) {
    if (!init() || g_ntimers >= kTimers || ms <= 0) return 0;
    QTimer* t = new QTimer(qApp);
    t->setInterval(ms);
    QObject::connect(t, &QTimer::timeout, t, [fn, user] { if (fn) fn(user); });
    t->start();
    g_timers[g_ntimers] = t;
    return ++g_ntimers;
}

void QtApi::stoptimer(int id) {
    if (id < 1 || id > g_ntimers || !g_timers[id - 1]) return;
    g_timers[id - 1]->stop();
    g_timers[id - 1]->deleteLater();
    g_timers[id - 1] = nullptr;
}

}  // namespace evgx
