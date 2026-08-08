#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: WindowManager                                          ║
// ║  Creates and tracks all native windows.                         ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "window_base.hpp"
#include <memory>
#include <vector>

namespace vgui {

class WindowManager {
public:
    static WindowManager& get();

    void init();
    void shutdown();

    /// Create a new platform window. The manager owns the lifetime.
    WindowBase& create(const std::string& title = "vgui",
                       int width  = 1280,
                       int height = 720);

    void destroy(WindowBase& window);
    void poll_all();

    bool any_open() const;
    size_t count() const;

private:
    WindowManager() = default;
    std::vector<std::unique_ptr<WindowBase>> m_windows;
};

} // namespace vgui
