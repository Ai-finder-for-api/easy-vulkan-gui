// Stopwatch on evg's monotonic clock, and a scope timer that accumulates into a counter.
#pragma once
#include <evg/evg.hpp>

namespace evgx { namespace timer {

class Stopwatch {
public:
    Stopwatch() : start_(evg.time()) {}
    void restart() { start_ = evg.time(); }
    double seconds() const { return evg.time() - start_; }
    double millis() const { return seconds() * 1000.0; }
private:
    double start_;
};

// adds the elapsed time of its scope to `total` (seconds) when destroyed
class Scoped {
public:
    explicit Scoped(double& total) : total_(total), start_(evg.time()) {}
    ~Scoped() { total_ += evg.time() - start_; }
private:
    double& total_;
    double start_;
};

}}  // namespace evgx::timer
