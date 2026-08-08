#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Timer                                                  ║
// ║  High-resolution clock and frame-time tracker.                  ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <chrono>

namespace vgui {

class Timer {
public:
    using Clock     = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    Timer();
    void  reset();

    double elapsed_s ()  const;   ///< Seconds since reset
    double elapsed_ms()  const;   ///< Milliseconds since reset
    float  elapsed_sf()  const;   ///< Seconds as float

private:
    TimePoint m_start;
};

class FrameTimer {
public:
    static FrameTimer& get();

    void  tick();               ///< Call once per frame
    float dt()          const;  ///< Delta time in seconds
    float fps()         const;  ///< Rolling average FPS
    float frame_ms()    const;  ///< Frame time in milliseconds
    double total_s()    const;  ///< Total elapsed seconds since first tick

private:
    FrameTimer() = default;
    Timer  m_clock;
    double m_prev_time  = 0;
    double m_total_time = 0;
    float  m_dt         = 0;
    float  m_fps        = 0;
    int    m_frame_count= 0;
    double m_fps_accum  = 0;
};

} // namespace vgui
