// Logging with levels and a pluggable sink. Nothing is printed unless the application installs a sink.
#pragma once
#include <stddef.h>

namespace evgx { namespace log {

enum class Level : int { Debug = 0, Info, Warn, Error, Off };

using Sink = void (*)(void* user, Level level, const char* message, size_t length);

struct State {
    Sink sink = nullptr;
    void* user = nullptr;
    Level min = Level::Info;
};

inline State& state() { static State s; return s; }

// installs the sink (null disables logging) and the minimum level that gets through
inline void set_sink(Sink sink, void* user = nullptr, Level min = Level::Info) {
    State& s = state();
    s.sink = sink;
    s.user = user;
    s.min = min;
}

inline void write(Level level, const char* message) {
    State& s = state();
    if (!s.sink || (int)level < (int)s.min) return;
    size_t n = 0;
    while (message[n]) n++;
    s.sink(s.user, level, message, n);
}

inline void debug(const char* m) { write(Level::Debug, m); }
inline void info(const char* m) { write(Level::Info, m); }
inline void warn(const char* m) { write(Level::Warn, m); }
inline void error(const char* m) { write(Level::Error, m); }

}}  // namespace evgx::log
