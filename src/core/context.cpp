// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Context  –  implementation stub                        ║
// ║  Real body lives in src/backend/vulkan/vk_device.cpp            ║
// ║  or src/backend/metal/mtl_device.mm depending on the platform.  ║
// ╚══════════════════════════════════════════════════════════════════╝
#include <vgui/core/context.hpp>
#include <vgui/core/logger.hpp>

namespace vgui {
struct Context::Impl {
    bool    ready       = false;
    uint32_t frame_idx  = 0;
};

Context& Context::get()              { static Context s; return s; }
void     Context::init()             { if (!m_impl) m_impl = std::make_unique<Impl>(); m_impl->ready = true; VGUI_INFO("GPU context ready"); }
void     Context::shutdown()         { if (m_impl) { m_impl->ready = false; } }
bool     Context::is_ready()   const { return m_impl && m_impl->ready; }
const char* Context::device_name() const { return "vgui GPU Device"; }
uint64_t Context::vram_bytes() const { return 0; }
uint32_t Context::frame_index()const { return m_impl ? m_impl->frame_idx : 0; }
void     Context::next_frame()       { if (m_impl) m_impl->frame_idx = (m_impl->frame_idx + 1) % 2; }
void     Context::wait_idle()        { /* GPU sync point */ }
} // namespace vgui
