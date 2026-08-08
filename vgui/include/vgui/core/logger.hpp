#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <mutex>
#include <functional>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace vgui {

/**
 * @brief Log severity levels
 */
enum class LogLevel : uint8_t {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5
};

/**
 * @brief Logger class for VGUI
 * 
 * Thread-safe logging with multiple output destinations.
 * Supports console, file, and callback-based logging.
 */
class Logger {
public:
    using LogCallback = std::function<void(LogLevel, const std::string&, const std::string&)>;
    
    /**
     * @brief Get the singleton instance
     */
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    
    /**
     * @brief Initialize the logger
     * @param minLevel Minimum log level to output
     * @param logFilePath Optional path to log file
     */
    void init(LogLevel minLevel = LogLevel::Info, 
              const std::optional<std::string>& logFilePath = std::nullopt) {
        std::lock_guard<std::mutex> lock(mutex_);
        minLevel_ = minLevel;
        
        if (logFilePath) {
            logFile_.open(logFilePath->c_str(), std::ios::out | std::ios::app);
        }
        
        VGUI_LOG_INFO("Logger initialized");
    }
    
    /**
     * @brief Set a custom log callback
     * @param callback Function called for each log message
     */
    void setCallback(LogCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        callback_ = callback;
    }
    
    /**
     * @brief Log a message at the given level
     */
    void log(LogLevel level, const std::string& category, const std::string& message) {
        if (level < minLevel_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Format the message
        std::string formatted = formatMessage(level, category, message);
        
        // Output to callback if set
        if (callback_) {
            callback_(level, category, message);
        }
        
        // Output to console
        std::cout << formatted << std::endl;
        
        // Output to file
        if (logFile_.is_open()) {
            logFile_ << formatted << std::endl;
            logFile_.flush();
        }
    }
    
    /**
     * @brief Set minimum log level
     */
    void setMinimumLevel(LogLevel level) {
        std::lock_guard<std::mutex> lock(mutex_);
        minLevel_ = level;
    }
    
    /**
     * @brief Flush any buffered output
     */
    void flush() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (logFile_.is_open()) {
            logFile_.flush();
        }
    }
    
    // Convenience logging methods
    void trace(const std::string& category, const std::string& message) { log(LogLevel::Trace, category, message); }
    void debug(const std::string& category, const std::string& message) { log(LogLevel::Debug, category, message); }
    void info(const std::string& category, const std::string& message) { log(LogLevel::Info, category, message); }
    void warning(const std::string& category, const std::string& message) { log(LogLevel::Warning, category, message); }
    void error(const std::string& category, const std::string& message) { log(LogLevel::Error, category, message); }
    void fatal(const std::string& category, const std::string& message) { log(LogLevel::Fatal, category, message); }
    
private:
    Logger() : minLevel_(LogLevel::Info) {}
    ~Logger() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::string formatMessage(LogLevel level, const std::string& category, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        
        ss << "[" << std::put_time(std::localtime(&time), "%H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) 
           << std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) % 1000 << "]";
        
        ss << " [" << levelToString(level) << "]";
        ss << " [" << category << "]";
        ss << " " << message;
        
        return ss.str();
    }
    
    const char* levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Trace:   return "TRACE";
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            default:                return "UNKNOWN";
        }
    }
    
    std::mutex mutex_;
    LogLevel minLevel_;
    LogCallback callback_;
    std::ofstream logFile_;
};

// Convenience macros
#define VGUI_LOG_TRACE(cat, msg) vgui::Logger::getInstance().trace(cat, msg)
#define VGUI_LOG_DEBUG(cat, msg) vgui::Logger::getInstance().debug(cat, msg)
#define VGUI_LOG_INFO(cat, msg)  vgui::Logger::getInstance().info(cat, msg)
#define VGUI_LOG_WARN(cat, msg)  vgui::Logger::getInstance().warning(cat, msg)
#define VGUI_LOG_ERROR(cat, msg) vgui::Logger::getInstance().error(cat, msg)
#define VGUI_LOG_FATAL(cat, msg) vgui::Logger::getInstance().fatal(cat, msg)

#ifdef VGUI_ENABLE_ASSERTS
    #define VGUI_ASSERT(condition, msg) \
        ((condition) ? (void)0 : vgui::Logger::getInstance().fatal("ASSERT", msg))
#else
    #define VGUI_ASSERT(condition, msg)
#endif

} // namespace vgui
