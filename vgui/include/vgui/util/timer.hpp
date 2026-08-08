#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

namespace vgui {

/**
 * @brief High-resolution timer
 */
class Timer {
public:
    /**
     * @brief Create and start timer
     */
    Timer();
    
    /**
     * @brief Start/restart timer
     */
    void start();
    
    /**
     * @brief Stop timer
     */
    void stop();
    
    /**
     * @brief Reset timer
     */
    void reset();
    
    /**
     * @brief Check if running
     */
    bool isRunning() const { return running_; }
    
    /**
     * @brief Get elapsed time in seconds
     */
    double elapsed() const;
    
    /**
     * @brief Get elapsed time in milliseconds
     */
    double elapsedMillis() const;
    
    /**
     * @brief Get elapsed time in microseconds
     */
    uint64_t elapsedMicros() const;
    
    /**
     * @brief Get elapsed time in nanoseconds
     */
    uint64_t elapsedNanos() const;
    
    /**
     * @brief Get last measured time
     */
    double getLast() const { return last_; }
    
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    
    TimePoint startTime_;
    TimePoint stopTime_;
    bool running_ = false;
    double last_ = 0.0;
    
    double elapsedSince(const TimePoint& start, const TimePoint& end) const;
};

/**
 * @brief Scoped timer (RAII)
 */
class ScopedTimer {
public:
    /**
     * @brief Create and start timer
     */
    ScopedTimer(const std::string& name);
    
    /**
     * @brief Create with callback
     */
    ScopedTimer(const std::string& name, std::function<void(const std::string&, double)> callback);
    
    /**
     * @brief Stop timer on destruction
     */
    ~ScopedTimer();
    
    /**
     * @brief Manually stop
     */
    void stop();
    
    /**
     * @brief Restart
     */
    void restart();
    
private:
    std::string name_;
    Timer timer_;
    std::function<void(const std::string&, double)> callback_;
    bool stopped_ = false;
};

/**
 * @brief FPS counter
 */
class FPSCounter {
public:
    /**
     * @brief Create FPS counter
     */
    FPSCounter();
    
    /**
     * @brief Update with frame time
     */
    void update(double frameTime);
    
    /**
     * @brief Get current FPS
     */
    float getFPS() const { return fps_; }
    
    /**
     * @brief Get average FPS
     */
    float getAverageFPS() const;
    
    /**
     * @brief Get minimum FPS
     */
    float getMinFPS() const { return minFPS_; }
    
    /**
     * @brief Get maximum FPS
     */
    float getMaxFPS() const { return maxFPS_; }
    
    /**
     * @brief Get frame time
     */
    float getFrameTime() const { return frameTime_; }
    
    /**
     * @brief Get average frame time
     */
    float getAverageFrameTime() const;
    
    /**
     * @brief Reset
     */
    void reset();
    
    /**
     * @brief Get frame count
     */
    uint64_t getFrameCount() const { return frameCount_; }
    
private:
    float fps_ = 0.0f;
    float frameTime_ = 0.0f;
    float minFPS_ = 1000.0f;
    float maxFPS_ = 0.0f;
    
    std::vector<float> frameTimes_;
    size_t frameTimeIndex_ = 0;
    size_t frameTimeCount_ = 0;
    uint64_t frameCount_ = 0;
    
    static const size_t MaxSamples = 120;
    
    void addFrameTime(float time);
    float calculateAverage() const;
};

/**
 * @brief Delta timer
 */
class DeltaTimer {
public:
    /**
     * @brief Create delta timer
     */
    DeltaTimer();
    
    /**
     * @brief Update to get delta time
     */
    float update();
    
    /**
     * @brief Get last delta time
     */
    float getDelta() const { return delta_; }
    
    /**
     * @brief Get smoothed delta time
     */
    float getSmoothedDelta(float smoothing = 0.1f);
    
    /**
     * @brief Set fixed timestep
     */
    void setFixedTimestep(float timestep);
    
    /**
     * @brief Get fixed timestep
     */
    float getFixedTimestep() const { return fixedTimestep_; }
    
    /**
     * @brief Check if fixed timestep is enabled
     */
    bool isFixedTimestep() const { return fixedTimestep_ > 0.0f; }
    
    /**
     * @brief Get accumulator
     */
    float getAccumulator() const { return accumulator_; }
    
    /**
     * @brief Reset accumulator
     */
    void resetAccumulator();
    
private:
    float delta_ = 0.0f;
    float accumulator_ = 0.0f;
    float fixedTimestep_ = 0.0f;  // 0 = variable
    float smoothedDelta_ = 0.0f;
    
    std::chrono::high_resolution_clock::time_point lastTime_;
};

/**
 * @brief Timing utilities
 */
namespace timing {

/**
 * @brief Get current time in seconds
 */
inline double now() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

/**
 * @brief Sleep for specified time
 */
inline void sleep(double seconds) {
    std::this_thread::sleep_for(
        std::chrono::duration<double>(seconds)
    );
}

/**
 * @brief Sleep for specified milliseconds
 */
inline void sleepMillis(double millis) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(millis))
    );
}

/**
 * @brief Get time difference
 */
inline double diff(double start, double end) {
    return end - start;
}

} // namespace timing

} // namespace vgui
