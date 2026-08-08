#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Buffer                                                 ║
// ║  GPU buffer (vertex, index, uniform, storage).                  ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <cstddef>
#include <memory>

namespace vgui {

enum class BufferUsage : uint8_t {
    Vertex,     ///< VBO
    Index,      ///< IBO (uint16 or uint32)
    Uniform,    ///< UBO - per-frame constants
    Storage,    ///< SSBO / Metal buffer
    Staging     ///< CPU-visible upload buffer
};

enum class BufferAccess : uint8_t {
    GpuOnly,    ///< Fastest; upload via staging
    CpuWrite,   ///< Mapped once at creation (dynamic data)
    CpuReadback ///< Readback from GPU to CPU
};

class Buffer {
public:
    static std::shared_ptr<Buffer> create(
        BufferUsage  usage,
        size_t       size_bytes,
        const void*  initial_data = nullptr,
        BufferAccess access       = BufferAccess::GpuOnly);

    ~Buffer();

    void   upload(const void* data, size_t bytes, size_t offset = 0);
    void*  map   ();
    void   unmap ();

    size_t      size_bytes()  const;
    BufferUsage usage()       const;
    bool        is_mapped()   const;
    void*       gpu_handle()  const;   ///< VkBuffer* / id<MTLBuffer>

private:
    Buffer() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
