// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: WindowManager  –  implementation                       ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/window/window_manager.hpp>
#include <vgui/core/logger.hpp>
#include <algorithm>

// Include the correct platform backend
#if defined(VGUI_PLATFORM_WINDOWS)
#  include "../../src/platform/windows/win32_window.hpp"
#elif defined(VGUI_PLATFORM_MACOS)
#  include "../../src/platform/macos/ns_window.hpp"
#elif defined(VGUI_PLATFORM_LINUX)
#  include "../../src/platform/linux/x11_window.hpp"
#endif

namespace vgui {

WindowManager& WindowManager::get() { static WindowManager s; return s; }

void WindowManager::init()     { VGUI_INFO("WindowManager initialised"); }
void WindowManager::shutdown() {
    m_windows.clear();
    VGUI_INFO("WindowManager shut down");
}

WindowBase& WindowManager::create(const std::string& title, int w, int h) {
#if defined(VGUI_PLATFORM_WINDOWS)
    auto win = std::make_unique<Win32Window>();
#elif defined(VGUI_PLATFORM_MACOS)
    auto win = std::make_unique<NSWindowImpl>();
#elif defined(VGUI_PLATFORM_LINUX)
    auto win = std::make_unique<X11Window>();
#else
    #error "No supported platform detected."
#endif
    win->create(title, w, h);
    win->show();
    m_windows.push_back(std::move(win));
    return *m_windows.back();
}

void WindowManager::destroy(WindowBase& window) {
    window.destroy();
    m_windows.erase(
        std::remove_if(m_windows.begin(), m_windows.end(),
            [&](const auto& p){ return p.get() == &window; }),
        m_windows.end());
}

void WindowManager::poll_all() {
    for (auto& w : m_windows) w->poll();
    // Remove windows that were closed by the user
    m_windows.erase(
        std::remove_if(m_windows.begin(), m_windows.end(),
            [](const auto& p){ return !p->is_open(); }),
        m_windows.end());
}

bool   WindowManager::any_open() const { return !m_windows.empty(); }
size_t WindowManager::count()    const { return m_windows.size();   }

} // namespace vgui
