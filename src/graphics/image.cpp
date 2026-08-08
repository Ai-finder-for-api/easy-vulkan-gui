// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Image  –  implementation  (built-in PNG/JPG decoder)   ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/graphics/image.hpp>
#include <vgui/core/logger.hpp>
#include <fstream>
#include <cstring>
#include <stdexcept>

// ── Tiny built-in PNG decoder (subset) ───────────────────────────────────────
// For production use, replace with a full stb_image-style decoder.
// This stub handles the interface; real decoding omitted for brevity.

namespace vgui {

Image::Image(int w, int h, PixelFormat fmt)
    : m_width(w), m_height(h), m_fmt(fmt) {
    m_data.resize(bytes_total(), 0);
}

int Image::channels() const {
    switch (m_fmt) {
        case PixelFormat::R8:     return 1;
        case PixelFormat::RG8:    return 2;
        case PixelFormat::RGB8:   return 3;
        case PixelFormat::RGBA8:  return 4;
        case PixelFormat::R32F:   return 1;
        case PixelFormat::RGBA32F:return 4;
        default: return 4;
    }
}

size_t Image::row_stride()  const { return static_cast<size_t>(m_width) * channels(); }
size_t Image::bytes_total() const { return row_stride() * static_cast<size_t>(m_height); }

std::shared_ptr<Image> Image::solid(int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    auto img = std::make_shared<Image>(w, h, PixelFormat::RGBA8);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            img->set_pixel(x, y, r, g, b, a);
    return img;
}

void Image::set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (x < 0 || y < 0 || x >= m_width || y >= m_height) return;
    size_t idx = (static_cast<size_t>(y) * m_width + x) * channels();
    m_data[idx+0] = r; m_data[idx+1] = g;
    m_data[idx+2] = b; if (channels() == 4) m_data[idx+3] = a;
}

void Image::get_pixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const {
    size_t idx = (static_cast<size_t>(y) * m_width + x) * channels();
    r = m_data[idx]; g = m_data[idx+1]; b = m_data[idx+2];
    a = channels() == 4 ? m_data[idx+3] : 255;
}

std::shared_ptr<Image> Image::load(const std::string& path) {
    // Stub: in a real build, use a built-in PNG/JPG parser here.
    VGUI_WARN("Image::load – built-in decoder stub, returning 1x1 white");
    return solid(1, 1, 255, 255, 255, 255);
}

std::shared_ptr<Image> Image::flipped_v() const {
    auto out = std::make_shared<Image>(m_width, m_height, m_fmt);
    size_t stride = row_stride();
    for (int y = 0; y < m_height; ++y)
        std::memcpy(out->m_data.data() + (m_height-1-y)*stride,
                    m_data.data() + y*stride, stride);
    return out;
}

bool Image::save_png(const std::string& /*path*/) const {
    VGUI_WARN("Image::save_png – stub not yet implemented");
    return false;
}
bool Image::save_jpg(const std::string& /*path*/, int) const {
    VGUI_WARN("Image::save_jpg – stub not yet implemented");
    return false;
}
std::shared_ptr<Image> Image::resized(int, int)    const { return {}; }
std::shared_ptr<Image> Image::to_format(PixelFormat) const { return {}; }

} // namespace vgui
