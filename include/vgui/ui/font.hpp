#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Font                                                   ║
// ║  Bitmap / SDF font atlas. Built-in fallback font included.      ║
// ╚══════════════════════════════════════════════════════════════════╝

#include "../graphics/texture.hpp"
#include "../math/vec2.hpp"
#include <string>
#include <memory>
#include <unordered_map>

namespace vgui {

struct Glyph {
    Vec2  uv_min, uv_max;     ///< Atlas UV coordinates
    Vec2  offset;             ///< Bearing (left, top)
    Vec2  size;               ///< Glyph size in pixels
    float advance = 0;        ///< Horizontal advance
};

class Font {
public:
    /// Load a TrueType / OTF font and rasterise at the given pixel size.
    /// Uses a built-in FreeType-free rasteriser.
    static std::shared_ptr<Font> load(const std::string& path, float size_px = 16.0f);

    /// Built-in 7x13 bitmap font (always available, no file needed)
    static std::shared_ptr<Font> builtin(float size_px = 13.0f);

    const Glyph* glyph(uint32_t codepoint) const;
    float line_height()  const;
    float ascender()     const;
    float descender()    const;
    float size_px()      const;

    Texture& atlas();

    /// Measure the pixel width of a UTF-8 string
    float measure_width(const std::string& text) const;

    /// Measure width capped at max_width, returning how many chars fit
    int   fit_chars    (const std::string& text, float max_width) const;

private:
    Font() = default;
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace vgui
