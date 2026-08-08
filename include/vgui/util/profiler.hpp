#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Profiler                                               ║
// ║  Lightweight CPU scope timer. Zero external deps.               ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    VGUI_PROFILE_SCOPE("render_pass");                           ║
// ║    VGUI_PROFILE_FUNC();                                         ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string>
#include <vector>
#include <chrono>
#include <cstdint>

namespace vgui {

struct ProfileEntry {
    std::string name;
    double      duration_ms = 0;
    int         depth       = 0;
    uint32_t    thread_id   = 0;
};

class Profiler {
public:
    static Profiler& get();

    void begin_frame();
    void end_frame();

    void push(const char* name);
    void pop();

    const std::vector<ProfileEntry>& entries() const;

    bool enabled = true;

private:
    Profiler() = default;
    struct Scope {
        const char* name;
        std::chrono::high_resolution_clock::time_point start;
        int depth;
    };
    std::vector<Scope>        m_stack;
    std::vector<ProfileEntry> m_entries;
    std::vector<ProfileEntry> m_prev_entries;
    int m_depth = 0;
};

class ProfileScope {
public:
    explicit ProfileScope(const char* name) { Profiler::get().push(name); }
    ~ProfileScope()                         { Profiler::get().pop();       }
};

} // namespace vgui

#define VGUI_PROFILE_SCOPE(name) ::vgui::ProfileScope _vgui_ps_##__LINE__{name}
#define VGUI_PROFILE_FUNC()      VGUI_PROFILE_SCOPE(__func__)
