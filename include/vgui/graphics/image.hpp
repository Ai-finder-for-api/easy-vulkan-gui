#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Image                                                  ║
// ║  CPU-side pixel data. Zero external deps - built-in PNG/JPG.   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <memory>

namespace vgui {

enum class PixelFormat : uint8_t { R8, RG8, RGB8, RGBA8, R32F, RGBA32F };

class Image {
public:
    Image() = default;
    Image(int width, int height, PixelFormat fmt);

    /// Load PNG / JPG / BMP / TGA / HDR via built-in decoder
    static std::shared_ptr<Image> load(const std::string& path);

    /// Create a solid-colour image
    static std::shared_ptr<Image> solid(int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    // ── Attributes ────────────────────────────────────────
    int          width()       const { return m_width;  }
    int          height()      const { return m_height; }
    PixelFormat  format()      const { return m_fmt;    }
    int          channels()    const;
    size_t       bytes_total() const;
    size_t       row_stride()  const;

    // ── Pixel access ──────────────────────────────────────
    const uint8_t* data()      const { return m_data.data(); }
    uint8_t*       data()            { return m_data.data(); }

    void  set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void  get_pixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;

    // ── Operations ────────────────────────────────────────
    std::shared_ptr<Image> resized   (int new_w, int new_h) const;
    std::shared_ptr<Image> flipped_v ()                     const;
    std::shared_ptr<Image> to_format (PixelFormat fmt)      const;

    // ── Save ──────────────────────────────────────────────
    bool save_png(const std::string& path) const;
    bool save_jpg(const std::string& path, int quality = 90) const;

private:
    int              m_width  = 0;
    int              m_height = 0;
    PixelFormat      m_fmt    = PixelFormat::RGBA8;
    std::vector<uint8_t> m_data;
};

} // namespace vgui
