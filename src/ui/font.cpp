#include <vgui/ui/font.hpp>
#include <vgui/core/logger.hpp>

namespace vgui {

struct Font::Impl {
    float size = 13.0f;
    float line_h = 16.0f;
    std::unordered_map<uint32_t, Glyph> glyphs;
    std::shared_ptr<Texture> atlas_tex;
};

std::shared_ptr<Font> Font::load(const std::string& path, float size_px) {
    VGUI_WARN("Font::load – TTF rasteriser stub");
    return builtin(size_px);
}

std::shared_ptr<Font> Font::builtin(float size_px) {
    auto f = std::shared_ptr<Font>(new Font);
    f->m_impl = std::make_unique<Impl>();
    f->m_impl->size   = size_px;
    f->m_impl->line_h = size_px * 1.2f;
    // Populate with dummy glyphs for ASCII printable range
    for (uint32_t cp = 32; cp < 127; ++cp) {
        Glyph g;
        g.advance = size_px * 0.6f;
        g.size    = { size_px * 0.6f, size_px };
        f->m_impl->glyphs[cp] = g;
    }
    return f;
}

const Glyph* Font::glyph(uint32_t cp) const {
    auto it = m_impl->glyphs.find(cp);
    return it != m_impl->glyphs.end() ? &it->second : nullptr;
}

float Font::line_height() const { return m_impl->line_h; }
float Font::ascender()    const { return m_impl->size * 0.8f; }
float Font::descender()   const { return m_impl->size * 0.2f; }
float Font::size_px()     const { return m_impl->size; }

float Font::measure_width(const std::string& text) const {
    float w = 0;
    for (unsigned char c : text) {
        auto* g = glyph(c);
        if (g) w += g->advance;
    }
    return w;
}

} // namespace vgui
