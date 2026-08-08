#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Logger                                                 ║
// ║  Lightweight thread-safe logger. Zero external deps.            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string>
#include <string_view>
#include <memory>

namespace vgui {

enum class LogLevel : uint8_t {
    Trace    = 0,
    Debug    = 1,
    Info     = 2,
    Warn     = 3,
    Error    = 4,
    Critical = 5,
    Off      = 6
};

class Logger {
public:
    static Logger& get();

    void     set_level(LogLevel level);
    LogLevel level()   const;
    void     enable_file(const std::string& path);

    void trace   (std::string_view msg);
    void debug   (std::string_view msg);
    void info    (std::string_view msg);
    void warn    (std::string_view msg);
    void error   (std::string_view msg);
    void critical(std::string_view msg);

private:
    Logger();
    ~Logger();
    void log(LogLevel level, std::string_view msg);

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui

#define VGUI_TRACE(msg)    ::vgui::Logger::get().trace   (msg)
#define VGUI_DEBUG(msg)    ::vgui::Logger::get().debug   (msg)
#define VGUI_INFO(msg)     ::vgui::Logger::get().info    (msg)
#define VGUI_WARN(msg)     ::vgui::Logger::get().warn    (msg)
#define VGUI_ERROR(msg)    ::vgui::Logger::get().error   (msg)
#define VGUI_CRITICAL(msg) ::vgui::Logger::get().critical(msg)
