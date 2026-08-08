#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Renderer                                               ║
// ║  High-level draw API. Wraps Vulkan or Metal.                    ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <memory>
#include <cstdint>
#include "../math/color.hpp"

namespace vgui {

class Renderer {
public:
    static Renderer& get();

    void init(class Window& window);
    void shutdown();

    /// Begin a new frame. Call once per loop iteration.
    bool begin_frame();

    /// Submit and present. Call once per loop iteration.
    void end_frame();

    /// Clear the colour buffer
    void clear(Color color = Color::black());

    /// GPU backend name  e.g. "Vulkan 1.3" / "Metal 3"
    const char* backend_name() const;

    /// Frames per second (rolling average)
    float fps()         const;
    float frame_time()  const; ///< milliseconds

private:
    Renderer() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
