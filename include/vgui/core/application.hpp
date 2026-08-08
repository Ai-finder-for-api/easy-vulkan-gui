#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Application                                            ║
// ║  Entry point singleton. One per process.                        ║
// ║                                                                 ║
// ║  Minimal usage:                                                 ║
// ║    #include <vgui/vgui.hpp>                                     ║
// ║    int main() {                                                 ║
// ║        auto& win = vgui::make_screen("Hello", 1280, 720);       ║
// ║        vgui::run();                                             ║
// ║    }                                                            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "config.hpp"
#include "window.hpp"
#include <functional>
#include <memory>

namespace vgui {

class Application {
public:
    static Application& get();

    /// Initialise platform, GPU context and renderer.
    void init(AppConfig cfg = {});

    /// Create a native window. Returns a stable reference.
    Window& make_window(const std::string& title = "vgui",
                        int width  = 1280,
                        int height = 720);

    /// Block and run the event loop until all windows are closed.
    void run();

    /// Request a clean exit from inside a callback.
    void quit();

    bool is_running() const;

    /// Optional per-frame user callback
    std::function<void(float dt)> on_update;

    /// Optional shutdown callback
    std::function<void()>         on_shutdown;

private:
    Application() = default;
    ~Application();
    void shutdown();

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
