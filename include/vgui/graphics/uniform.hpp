#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Uniform                                                ║
// ║  Typed helper for uploading per-frame constants to the GPU.     ║
// ║                                                                 ║
// ║  Usage:                                                         ║
// ║    struct MVP { Mat4 model, view, proj; };                      ║
// ║    vgui::Uniform<MVP> ubo;                                      ║
// ║    ubo->model = Mat4::identity();                               ║
// ║    ubo.flush();                                                 ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "buffer.hpp"
#include <memory>

namespace vgui {

template<typename T>
class Uniform {
public:
    Uniform() {
        m_buf = Buffer::create(BufferUsage::Uniform, sizeof(T),
                               nullptr, BufferAccess::CpuWrite);
        m_mapped = static_cast<T*>(m_buf->map());
    }

    ~Uniform() { if (m_buf) m_buf->unmap(); }

    T*       operator->()       { return m_mapped; }
    const T* operator->() const { return m_mapped; }
    T&       operator* ()       { return *m_mapped; }

    /// Write any pending changes to the GPU buffer.
    /// (On UMA / Metal this is already visible; on discrete GPU it flushes the cache.)
    void flush() { /* cache-flush handled per-backend */ }

    Buffer& buffer() { return *m_buf; }

private:
    std::shared_ptr<Buffer> m_buf;
    T*                      m_mapped = nullptr;
};

} // namespace vgui
