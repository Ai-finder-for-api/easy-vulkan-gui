// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Profiler  –  implementation                            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/util/profiler.hpp>
#include <thread>

namespace vgui {

Profiler& Profiler::get() { static Profiler s; return s; }

void Profiler::begin_frame() { m_entries.clear(); m_depth = 0; }
void Profiler::end_frame()   { m_prev_entries = m_entries; }

void Profiler::push(const char* name) {
    if (!enabled) return;
    m_stack.push_back({ name, std::chrono::high_resolution_clock::now(), m_depth++ });
}

void Profiler::pop() {
    if (!enabled || m_stack.empty()) return;
    auto& s  = m_stack.back();
    auto  dur = std::chrono::high_resolution_clock::now() - s.start;
    double ms = std::chrono::duration<double, std::milli>(dur).count();
    m_entries.push_back({
        s.name, ms, s.depth,
        static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()))
    });
    m_stack.pop_back();
    --m_depth;
}

const std::vector<ProfileEntry>& Profiler::entries() const { return m_prev_entries; }

} // namespace vgui
