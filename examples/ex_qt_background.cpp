// ex_qt_background: a background job, a repeating timer and persisted settings, reported as they complete.
#include <stdio.h>
#include <evg/evg.hpp>

struct Job { int result; bool done; };

static void compute(void* u) {
    // stands in for slow work: sums numbers on a worker thread
    Job* j = static_cast<Job*>(u);
    long long s = 0;
    for (int i = 1; i <= 2000000; i++) s += i;
    j->result = (int)(s % 1000000);
}

static void finished(void* u) {
    Job* j = static_cast<Job*>(u);
    j->done = true;
    printf("background job finished: result %d\n", j->result);
}

static int g_ticks = 0;
static void tick(void*) { g_ticks++; }

int main() {
    if (!evg.qt.init()) { printf("qt: %s\n", evg.error()); return 1; }
    evg.qt.setapp("evg-example", "background");
    evg.qt.setsetting("runs", "1");
    printf("stored setting: %s\n", evg.qt.getsetting("runs", "0"));

    int timer = evg.qt.createtimer(10, tick, nullptr);

    Job job = {0, false};
    evg.qt.runasync(compute, &job, finished, &job);
    double t0 = evg.time();
    while (!job.done && evg.time() - t0 < 5.0) evg.qt.pump();

    evg.qt.stoptimer(timer);
    printf("timer fired while waiting: %s\n", g_ticks > 0 ? "yes" : "no");
    return job.done ? 0 : 1;
}
