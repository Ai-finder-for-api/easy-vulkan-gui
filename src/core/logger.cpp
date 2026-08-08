// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Logger  –  implementation                              ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/core/logger.hpp>
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>

namespace vgui {

struct Logger::Impl {
    LogLevel       level { LogLevel::Info };
    std::ofstream  file;
    std::mutex     mtx;
};

Logger& Logger::get() {
    static Logger s_instance;
    return s_instance;
}

Logger::Logger()  : m_impl(std::make_unique<Impl>()) {}
Logger::~Logger() = default;

void Logger::set_level(LogLevel level) { m_impl->level = level; }
LogLevel Logger::level() const         { return m_impl->level;  }

void Logger::enable_file(const std::string& path) {
    std::lock_guard lock(m_impl->mtx);
    m_impl->file.open(path, std::ios::app);
}

static const char* level_str(LogLevel l) {
    switch (l) {
        case LogLevel::Trace:    return "TRACE";
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO ";
        case LogLevel::Warn:     return "WARN ";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT ";
        default:                 return "?????";
    }
}

void Logger::log(LogLevel level, std::string_view msg) {
    if (level < m_impl->level) return;
    std::lock_guard lock(m_impl->mtx);

    // Timestamp
    std::time_t t = std::time(nullptr);
    char ts[20];
    std::strftime(ts, sizeof(ts), "%H:%M:%S", std::localtime(&t));

    // Format: [HH:MM:SS] [LEVEL] message
    std::string line = std::string("[") + ts + "] [" + level_str(level) + "] " + std::string(msg) + "\n";

    std::cout << line;
    if (m_impl->file.is_open()) m_impl->file << line;
}

void Logger::trace   (std::string_view m) { log(LogLevel::Trace,    m); }
void Logger::debug   (std::string_view m) { log(LogLevel::Debug,    m); }
void Logger::info    (std::string_view m) { log(LogLevel::Info,     m); }
void Logger::warn    (std::string_view m) { log(LogLevel::Warn,     m); }
void Logger::error   (std::string_view m) { log(LogLevel::Error,    m); }
void Logger::critical(std::string_view m) { log(LogLevel::Critical, m); }

} // namespace vgui
