// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Application  –  implementation                         ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/core/application.hpp>
#include <vgui/core/logger.hpp>
#include <vgui/core/context.hpp>
#include <vgui/core/renderer.hpp>
#include <vgui/window/window_manager.hpp>
#include <vgui/input/input_manager.hpp>
#include <vgui/util/timer.hpp>

namespace vgui {

struct Application::Impl {
    AppConfig cfg;
    bool      running   = false;
    Window*   main_win  = nullptr;
};

Application& Application::get() {
    static Application s_instance;
    return s_instance;
}

Application::~Application() { shutdown(); }

void Application::init(AppConfig cfg) {
    if (!m_impl) m_impl = std::make_unique<Impl>();
    m_impl->cfg = cfg;

    if (cfg.log_file) Logger::get().enable_file(cfg.log_path);
    VGUI_INFO("vgui initialising...");

    WindowManager::get().init();
    Context::get().init();
}

Window& Application::make_window(const std::string& title, int w, int h) {
    if (!m_impl) init({});
    // Delegate to the platform WindowManager
    auto& wb = WindowManager::get().create(title, w, h);
    // Wrap in our public Window handle (simplified; real impl maps wb → Window)
    static Window s_win;   // placeholder — real impl uses a pool
    return s_win;
}

void Application::run() {
    if (!m_impl) init({});
    m_impl->running = true;
    VGUI_INFO("Entering event loop");

    FrameTimer& ft = FrameTimer::get();

    while (m_impl->running && WindowManager::get().any_open()) {
        ft.tick();
        float dt = ft.dt();

        WindowManager::get().poll_all();
        InputManager::get().update();

        if (on_update) on_update(dt);

        Renderer::get().begin_frame();
        Renderer::get().end_frame();

        InputManager::get().end_frame();
    }

    shutdown();
}

void Application::quit() {
    if (m_impl) m_impl->running = false;
}

bool Application::is_running() const {
    return m_impl && m_impl->running;
}

void Application::shutdown() {
    if (!m_impl) return;
    VGUI_INFO("vgui shutting down");
    if (on_shutdown) on_shutdown();
    Context::get().wait_idle();
    Renderer::get().shutdown();
    Context::get().shutdown();
    WindowManager::get().shutdown();
    m_impl.reset();
}

} // namespace vgui
