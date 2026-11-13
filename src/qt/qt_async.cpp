// Background work: runs a function on the thread pool, then a completion callback on the GUI thread.
#include "qt_common.hpp"
#include <QApplication>
#include <QMetaObject>
#include <QRunnable>
#include <QThreadPool>

namespace evgx {

namespace {
// a QRunnable: runs the work on a worker thread, then queues the completion callback to the GUI thread
class Job : public QRunnable {
public:
    Job(void (*fn)(void*), void* user, void (*done)(void*), void* duser) : fn_(fn), user_(user), done_(done), duser_(duser) {}
    void run() override {
        if (fn_) fn_(user_);
        void (*d)(void*) = done_;
        void* du = duser_;
        if (d) QMetaObject::invokeMethod(qApp, [d, du] { d(du); }, Qt::QueuedConnection);
    }
private:
    void (*fn_)(void*);
    void* user_;
    void (*done_)(void*);
    void* duser_;
};
}

bool QtApi::runasync(void (*fn)(void*), void* user, void (*done)(void*), void* duser) {
    if (!init()) return false;
    QThreadPool::globalInstance()->start(new Job(fn, user, done, duser));   // the pool deletes the job after it runs
    return true;
}

}  // namespace evgx
