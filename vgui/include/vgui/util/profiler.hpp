#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <functional>

#ifdef VGUI_ENABLE_PROFILING
    #define VGUI_PROFILE_SCOPE(name) vgui::Profiler::get().beginScope(name, __LINE__, __FILE__)
    #define VGUI_PROFILE_FUNCTION() VGUI_PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
    #define VGUI_PROFILE_SCOPE(name)
    #define VGUI_PROFILE_FUNCTION()
#endif

namespace vgui {

/**
 * @brief Profiler scopes
 */
enum class ProfileScopeType {
    Function,
    Region,
    Custom
};

/**
 * @brief Profiler event types
 */
enum class ProfileEventType {
    Begin,
    End,
    Instant,
    Counter
};

/**
 * @brief Profiled section
 */
struct ProfileSection {
    std::string name;
    std::string file;
    int line = 0;
    std::chrono::high_resolution_clock::time_point begin;
    std::chrono::high_resolution_clock::time_point end;
    ProfileScopeType type = ProfileScopeType::Region;
    
    ProfileSection() = default;
    
    ProfileSection(const std::string& name, const std::string& file, int line)
        : name(name), file(file), line(line) {}
    
    double duration() const {
        auto diff = end - begin;
        return std::chrono::duration<double, std::milli>(diff).count();
    }
};

/**
 * @brief Profiler sample
 */
struct ProfileSample {
    std::string name;
    double duration = 0.0;
    double selfDuration = 0.0;
    int callCount = 0;
    double minDuration = 0.0;
    double maxDuration = 0.0;
    double avgDuration = 0.0;
    
    ProfileSample() = default;
    ProfileSample(const std::string& name) : name(name) {}
};

/**
 * @brief Frame timing info
 */
struct FrameTiming {
    double frameTime = 0.0;
    double cpuTime = 0.0;
    double gpuTime = 0.0;
    
    // Breakdown
    double renderTime = 0.0;
    double computeTime = 0.0;
    double presentTime = 0.0;
    double inputTime = 0.0;
    double updateTime = 0.0;
    
    uint64_t frameNumber = 0;
    double timestamp = 0.0;
};

/**
 * @brief Profiler class
 */
class Profiler {
public:
    /**
     * @brief Get singleton instance
     */
    static Profiler& get();
    
    /**
     * @brief Begin profiling scope
     */
    void beginScope(const std::string& name, int line = 0, 
                   const std::string& file = "");
    
    /**
     * @brief End profiling scope
     */
    void endScope(const std::string& name);
    
    /**
     * @brief Push custom scope
     */
    std::shared_ptr<ProfileSection> pushScope(const std::string& name,
                                              ProfileScopeType type = ProfileScopeType::Custom);
    
    /**
     * @brief Pop scope
     */
    void popScope();
    
    /**
     * @brief Record instant event
     */
    void recordInstant(const std::string& name, double value);
    
    /**
     * @brief Record counter
     */
    void recordCounter(const std::string& name, double value);
    
    /**
     * @brief Record event
     */
    void recordEvent(ProfileEventType type, const std::string& name, double value = 0.0);
    
    /**
     * @brief Start frame recording
     */
    void beginFrame();
    
    /**
     * @brief End frame recording
     */
    void endFrame();
    
    /**
     * @brief Get frame timing
     */
    const FrameTiming& getFrameTiming() const { return frameTiming_; }
    
    /**
     * @brief Get samples
     */
    const std::vector<ProfileSample>& getSamples() const { return samples_; }
    
    /**
     * @brief Get sample by name
     */
    const ProfileSample* getSample(const std::string& name) const;
    
    /**
     * @brief Clear samples
     */
    void clearSamples();
    
    /**
     * @brief Set enabled
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if enabled
     */
    bool isEnabled() const { return enabled_; }
    
    /**
     * @brief Set max samples to keep
     */
    void setMaxSamples(size_t max);
    
    /**
     * @brief Get max samples
     */
    size_t getMaxSamples() const { return maxSamples_; }
    
    /**
     * @brief Reset profiler
     */
    void reset();
    
    /**
     * @brief Get total time
     */
    double getTotalTime() const { return totalTime_; }
    
    /**
     * @brief Get frame count
     */
    uint64_t getFrameCount() const { return frameCount_; }
    
    /**
     * @brief Set GPU timing callback
     */
    void setGPUDurationCallback(std::function<double(const std::string&)> callback);
    
    /**
     * @brief Set output callback
     */
    void setOutputCallback(std::function<void(const std::string&)> callback);
    
    /**
     * @brief Export to JSON
     */
    std::string exportToJSON() const;
    
    /**
     * @brief Export to CSV
     */
    std::string exportToCSV() const;
    
    /**
     * @brief Get current frame number
     */
    uint64_t getCurrentFrame() const { return currentFrame_; }
    
private:
    Profiler();
    ~Profiler();
    
    struct ScopeStackEntry {
        std::shared_ptr<ProfileSection> section;
        std::string name;
    };
    
    std::vector<ScopeStackEntry> scopeStack_;
    std::vector<ProfileSample> samples_;
    std::unordered_map<std::string, ProfileSample> sampleMap_;
    
    FrameTiming frameTiming_;
    double totalTime_ = 0.0;
    uint64_t frameCount_ = 0;
    uint64_t currentFrame_ = 0;
    
    bool enabled_ = false;
    size_t maxSamples_ = 1000;
    
    std::chrono::high_resolution_clock::time_point frameStart_;
    std::chrono::high_resolution_clock::time_point lastTimestamp_;
    
    std::function<double(const std::string&)> gpuCallback_;
    std::function<void(const std::string&)> outputCallback_;
    
    std::mutex mutex_;
    
    void updateSample(const std::string& name, double duration);
    void finalizeSample(const std::string& name, double duration);
    double getCurrentTime() const;
    
    static Profiler instance_;
};

/**
 * @brief Scoped profiling helper
 */
class ScopedProfile {
public:
    ScopedProfile(const std::string& name, ProfileScopeType type = ProfileScopeType::Region);
    ~ScopedProfile();
    
private:
    std::string name_;
};

/**
 * @brief CPU timer helper
 */
class CPUTimer {
public:
    CPUTimer();
    ~CPUTimer();
    
    void start();
    void stop();
    double elapsed() const;
    
private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
    bool running_ = false;
};

/**
 * @brief GPU timer wrapper
 */
class GPUTimer {
public:
    GPUTimer();
    ~GPUTimer();
    
    void begin();
    void end();
    double elapsed() const;
    
private:
    // Platform-specific GPU timing
    void* queryPool_ = nullptr;
    bool active_ = false;
};

/**
 * @brief Profiler markers (for visualizations)
 */
namespace profiler {

inline void marker(const std::string& name) {
    Profiler::get().recordInstant(name, Profiler::get().getCurrentTime());
}

inline void beginMarker(const std::string& name) {
    Profiler::get().beginScope(name);
}

inline void endMarker() {
    // Will be handled by scope
}

} // namespace profiler

} // namespace vgui
