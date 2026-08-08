// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Renderer  –  implementation stub                       ║
// ╚══════════════════════════════════════════════════════════════════╝
#include <vgui/core/renderer.hpp>
#include <vgui/core/logger.hpp>
#include <vgui/util/timer.hpp>
namespace vgui {
struct Renderer::Impl { bool in_frame = false; };
Renderer& Renderer::get()            { static Renderer s; return s; }
void Renderer::init(class Window&)   { if (!m_impl) m_impl = std::make_unique<Impl>(); VGUI_INFO("Renderer init"); }
void Renderer::shutdown()            { m_impl.reset(); }
bool Renderer::begin_frame()         { if (m_impl) { m_impl->in_frame = true; } return true; }
void Renderer::end_frame()           { if (m_impl) m_impl->in_frame = false; }
void Renderer::clear(Color)          {}
const char* Renderer::backend_name() const {
#if defined(VGUI_BACKEND_VULKAN)
    return "Vulkan 1.3";
#elif defined(VGUI_BACKEND_METAL)
    return "Metal 3";
#else
    return "None";
#endif
}
float Renderer::fps()        const { return FrameTimer::get().fps(); }
float Renderer::frame_time() const { return FrameTimer::get().frame_ms(); }
} // namespace vgui
