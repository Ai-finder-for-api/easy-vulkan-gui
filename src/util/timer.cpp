// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Timer  –  implementation                               ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/util/timer.hpp>

namespace vgui {

Timer::Timer() : m_start(Clock::now()) {}

void   Timer::reset()        { m_start = Clock::now(); }
double Timer::elapsed_s()  const { return std::chrono::duration<double>(Clock::now()-m_start).count(); }
double Timer::elapsed_ms() const { return elapsed_s() * 1000.0; }
float  Timer::elapsed_sf() const { return static_cast<float>(elapsed_s()); }

FrameTimer& FrameTimer::get() { static FrameTimer s; return s; }

void FrameTimer::tick() {
    double now = m_clock.elapsed_s();
    m_dt        = static_cast<float>(now - m_prev_time);
    m_prev_time = now;
    m_total_time= now;

    m_fps_accum += m_dt;
    m_frame_count++;
    if (m_fps_accum >= 0.5) {
        m_fps         = static_cast<float>(m_frame_count / m_fps_accum);
        m_fps_accum   = 0;
        m_frame_count = 0;
    }
}

float  FrameTimer::dt()       const { return m_dt;         }
float  FrameTimer::fps()      const { return m_fps;        }
float  FrameTimer::frame_ms() const { return m_dt * 1000.f;}
double FrameTimer::total_s()  const { return m_total_time; }

} // namespace vgui
