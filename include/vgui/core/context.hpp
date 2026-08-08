#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Context                                                ║
// ║  GPU context - holds device, queues, and allocator.             ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <memory>
#include <cstdint>

namespace vgui {

class Context {
public:
    static Context& get();

    void init();
    void shutdown();

    bool is_ready() const;

    /// GPU device name e.g. "NVIDIA GeForce RTX 4090"
    const char* device_name() const;

    /// Dedicated VRAM in bytes
    uint64_t vram_bytes() const;

    /// Current frame index (0..max_frames-1)
    uint32_t frame_index() const;

    /// Advance to next frame
    void next_frame();

    /// Wait for all GPU work to finish (use before shutdown)
    void wait_idle();

private:
    Context() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
