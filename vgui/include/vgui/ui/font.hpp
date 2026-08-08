#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "vgui/math/vec2.hpp"
#include "vgui/math/vec4.hpp"

#ifdef VGUI_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(VGUI_PLATFORM_LINUX)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif

#include <vulkan/vulkan.h>

namespace vgui {

/**
 * @brief Font characters (glyphs)
 */
struct Glyph {
    uint32_t character;
    uint32_t id;  // Glyph index in font
    
    float x, y;       // Position in texture atlas
    float width;      // Glyph width in atlas
    float height;     // Glyph height in atlas
    
    float xOffset;    // Offset from cursor position
    float yOffset;    // Offset from cursor position (baseline)
    float xAdvance;   // X advance to next character
    
    // Glyph bounding box
    float left;
    float top;
    float right;
    float bottom;
    
    // Texture coordinates
    float u, v;
    float u2, v2;
};

/**
 * @brief Font metrics
 */
struct FontMetrics {
    float ascender;      // Distance from baseline to top of ascender
    float descender;     // Distance from baseline to bottom of descender (negative)
    float lineHeight;    // Total height of a line of text
    float averageWidth;  // Average character width
    float maxAdvance;    // Maximum character advance
    
    float underlineThickness;
    float underlinePosition;
    float strikeoutThickness;
    float strikeoutPosition;
    
    uint32_t unitsPerEM;
    
    // Bounding box
    float bboxLeft;
    float bboxTop;
    float bboxRight;
    float bboxBottom;
};

/**
 * @brief Font style
 */
enum class FontStyle {
    Normal,
    Bold,
    Italic,
    BoldItalic
};

/**
 * @brief Font weight
 */
enum class FontWeight {
    Thin = 100,
    ExtraLight = 200,
    Light = 300,
    Normal = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900
};

/**
 * @brief Font stretch
 */
enum class FontStretch {
    UltraCondensed = 1,
    ExtraCondensed = 2,
    Condensed = 3,
    SemiCondensed = 4,
    Normal = 5,
    SemiExpanded = 6,
    Expanded = 7,
    ExtraExpanded = 8,
    UltraExpanded = 9
};

/**
 * @brief Font class
 * 
 * Manages font loading, glyph rendering, and text measurement.
 * Supports TTF/OTF font formats.
 */
class Font {
public:
    /**
     * @brief Create an empty font
     */
    Font();
    
    /**
     * @brief Destroy the font
     */
    ~Font();
    
    /**
     * @brief Check if font is valid
     */
    bool isValid() const { return valid_; }
    
    /**
     * @brief Get font name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Get font family
     */
    const std::string& getFamily() const { return family_; }
    
    /**
     * @brief Get font style
     */
    FontStyle getStyle() const { return style_; }
    
    /**
     * @brief Get font weight
     */
    FontWeight getWeight() const { return weight_; }
    
    /**
     * @brief Get font stretch
     */
    FontStretch getStretch() const { return stretch_; }
    
    /**
     * @brief Load font from file
     * @param path Path to font file
     * @param size Font size in pixels
     * @return true on success
     */
    bool loadFromFile(const std::string& path, float size = 16.0f);
    
    /**
     * @brief Load font from memory
     */
    bool loadFromMemory(const void* data, size_t size, float fontSize = 16.0f);
    
    /**
     * @brief Load font from memory with full path
     */
    bool loadFromMemory(const void* data, size_t size,
                       const std::string& path, float fontSize = 16.0f);
    
    /**
     * @brief Load font from system
     */
    bool loadFromSystem(const std::string& family,
                       FontStyle style = FontStyle::Normal,
                       FontWeight weight = FontWeight::Normal,
                       float size = 16.0f);
    
    /**
     * @brief Load multiple sizes (for resolution independence)
     */
    bool loadSizes(const std::string& path,
                  const std::vector<float>& sizes);
    
    /**
     * @brief Set font size
     */
    void setSize(float size);
    
    /**
     * @brief Get font size
     */
    float getSize() const { return size_; }
    
    /**
     * @brief Get default size
     */
    float getDefaultSize() const { return defaultSize_; }
    
    /**
     * @brief Get font metrics
     */
    const FontMetrics& getMetrics() const { return metrics_; }
    
    /**
     * @brief Get line height
     */
    float getLineHeight() const { return metrics_.lineHeight * size_; }
    
    /**
     * @brief Get ascender
     */
    float getAscender() const { return metrics_.ascender * size_; }
    
    /**
     * @brief Get descender
     */
    float getDescender() const { return metrics_.descender * size_; }
    
    /**
     * @brief Get character glyph
     */
    const Glyph* getGlyph(uint32_t character) const;
    
    /**
     * @brief Check if glyph exists
     */
    bool hasGlyph(uint32_t character) const;
    
    /**
     * @brief Get glyph bounding box
     */
    bool getGlyphBounds(uint32_t character,
                       float& left, float& top,
                       float& width, float& height) const;
    
    /**
     * @brief Get glyph texture coordinates
     */
    bool getGlyphTextureCoords(uint32_t character,
                              float& u, float& v,
                              float& u2, float& v2) const;
    
    /**
     * @brief Measure text width
     */
    float measureTextWidth(const std::string& text,
                          float fontSize = 0.0f) const;
    
    /**
     * @brief Measure text width (with explicit size)
     */
    float measureTextWidth(const std::string& text,
                          float fontSize, float sizeOverride) const;
    
    /**
     * @brief Measure text height
     */
    float measureTextHeight(const std::string& text,
                           float fontSize = 0.0f) const;
    
    /**
     * @brief Measure text (returns bounds)
     */
    Vec2f measureText(const std::string& text,
                     float fontSize = 0.0f) const;
    
    /**
     * @brief Get text bounds
     */
    Rectf getTextBounds(const std::string& text,
                       float fontSize = 0.0f) const;
    
    /**
     * @brief Get character advance
     */
    float getCharacterAdvance(uint32_t character,
                            float fontSize = 0.0f) const;
    
    /**
     * @brief Get kerning between characters
     */
    float getKerning(uint32_t char1, uint32_t char2,
                    float fontSize = 0.0f) const;
    
    /**
     * @brief Get number of glyphs
     */
    size_t getGlyphCount() const { return glyphs_.size(); }
    
    /**
     * @brief Get all characters
     */
    std::vector<uint32_t> getCharacters() const;
    
    /**
     * @brief Get texture atlas
     */
    VkImage getAtlasImage() const { return atlasImage_; }
    
    /**
     * @brief Get texture atlas view
     */
    VkImageView getAtlasImageView() const { return atlasImageView_; }
    
    /**
     * @brief Get atlas dimensions
     */
    Vec2u getAtlasSize() const { return Vec2u(atlasWidth_, atlasHeight_); }
    
    /**
     * @brief Check if atlas is valid
     */
    bool hasAtlas() const { return atlasValid_; }
    
    /**
     * @brief Generate mipmaps for atlas
     */
    bool generateMipmaps();
    
    /**
     * @brief Get font data pointer
     */
    const void* getFontData() const { return fontData_; }
    
    /**
     * @brief Get font data size
     */
    size_t getFontDataSize() const { return fontDataSize_; }
    
    /**
     * @brief Set fallback font
     */
    void setFallbackFont(std::shared_ptr<Font> font);
    
    /**
     * @brief Get fallback font
     */
    std::shared_ptr<Font> getFallbackFont() const { return fallbackFont_; }
    
    /**
     * @brief Add font variation
     */
    bool addVariation(const std::string& variation);
    
    /**
     * @brief Has font variations
     */
    bool hasVariations() const { return !variations_.empty(); }
    
    /**
     * @brief Get variations
     */
    const std::vector<std::string>& getVariations() const { return variations_; }
    
    /**
     * @brief Get OS/2 table info
     */
    uint16_t getFamilyClass() const { return familyClass_; }
    
    /**
     * @brief Check if monospaced
     */
    bool isMonospaced() const { return isMonospaced_; }
    
    /**
     * @brief Check if fixed pitch
     */
    bool isFixedPitch() const { return isFixedPitch_; }
    
    /**
     * @brief Get underline position
     */
    float getUnderlinePosition() const { 
        return metrics_.underlinePosition * size_; 
    }
    
    /**
     * @brief Get underline thickness
     */
    float getUnderlineThickness() const { 
        return metrics_.underlineThickness * size_; 
    }
    
    /**
     * @brief Create font from system default
     */
    static std::shared_ptr<Font> createDefault();
    
    /**
     * @brief Create font from file
     */
    static std::shared_ptr<Font> createFromFile(const std::string& path,
                                                float size = 16.0f);
    
    /**
     * @brief Create font from memory
     */
    static std::shared_ptr<Font> createFromMemory(const void* data, size_t size,
                                                   float size = 16.0f);
    
    /**
     * @brief Create font with multiple sizes
     */
    static std::shared_ptr<Font> createWithSizes(const std::string& path,
                                                 const std::vector<float>& sizes);
    
private:
    bool loadFont(const void* data, size_t size,
                 const std::string& path, float fontSize);
    
    bool buildAtlas();
    void findGlyphInAtlas(uint32_t character);
    bool addGlyphToAtlas(const Glyph& glyph);
    void clearAtlas();
    
    // TrueType/OpenType parsing
    bool parseFont(const void* data, size_t size);
    void parseTables();
    void parseHeadTable();
    void parseHheaTable();
    void parseMaxpTable();
    void parseOS2Table();
    void parseHmtxTable();
    void parseKernTable();
    void parseCmapTable();
    void parseGlyfTable();
    void parseLocaTable();
    
    bool valid_ = false;
    std::string name_;
    std::string family_;
    FontStyle style_ = FontStyle::Normal;
    FontWeight weight_ = FontWeight::Normal;
    FontStretch stretch_ = FontStretch::Normal;
    
    float size_ = 16.0f;
    float defaultSize_ = 16.0f;
    
    // Raw font data
    const void* fontData_ = nullptr;
    size_t fontDataSize_ = 0;
    bool ownsData_ = false;
    
    // Parsed tables
    std::unordered_map<uint32_t, std::vector<uint8_t>> tables_;
    
    // Metrics
    FontMetrics metrics_;
    
    // Glyphs
    std::unordered_map<uint32_t, Glyph> glyphs_;
    Glyph missingGlyph_;
    
    // Atlas
    VkImage atlasImage_ = VK_NULL_HANDLE;
    VkImageView atlasImageView_ = VK_NULL_HANDLE;
    VkDeviceMemory atlasMemory_ = VK_NULL_HANDLE;
    uint32_t atlasWidth_ = 0;
    uint32_t atlasHeight_ = 0;
    bool atlasValid_ = false;
    
    // Fallback
    std::shared_ptr<Font> fallbackFont_;
    
    // Variations
    std::vector<std::string> variations_;
    
    // Font info
    uint16_t familyClass_ = 0;
    bool isMonospaced_ = false;
    bool isFixedPitch_ = false;
    
    // Texture coordinates for atlas
    float atlasU_ = 0.0f;
    float atlasV_ = 0.0f;
    float atlasScaleU_ = 1.0f;
    float atlasScaleV_ = 1.0f;
};

/**
 * @brief Font manager
 */
class FontManager {
public:
    /**
     * @brief Get instance
     */
    static FontManager& getInstance();
    
    /**
     * @brief Load font from file
     */
    std::shared_ptr<Font> load(const std::string& name,
                               const std::string& path,
                               float size = 16.0f);
    
    /**
     * @brief Load font from memory
     */
    std::shared_ptr<Font> loadFromMemory(const std::string& name,
                                         const void* data, size_t size,
                                         float sizeBytes = 16.0f);
    
    /**
     * @brief Get font by name
     */
    std::shared_ptr<Font> get(const std::string& name);
    
    /**
     * @brief Check if font exists
     */
    bool has(const std::string& name) const;
    
    /**
     * @brief Remove font
     */
    void remove(const std::string& name);
    
    /**
     * @brief Get all fonts
     */
    std::vector<std::string> getFontNames() const;
    
    /**
     * @brief Clear all fonts
     */
    void clear();
    
    /**
     * @brief Get default font
     */
    std::shared_ptr<Font> getDefault();
    
    /**
     * @brief Set default font
     */
    void setDefault(const std::string& name);
    
    /**
     * @brief Find font by family name
     */
    std::shared_ptr<Font> findFamily(const std::string& family);
    
    /**
     * @brief Get font count
     */
    size_t getFontCount() const { return fonts_.size(); }
    
private:
    FontManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Font>> fonts_;
    std::string defaultFontName_;
    
    void loadSystemFonts();
};

/**
 * @brief Text rendering utilities
 */
class TextRenderer {
public:
    /**
     * @brief Create text vertices for rendering
     */
    static std::vector<class UIVertex> createTextVertices(
        const std::string& text,
        const Vec2f& position,
        float fontSize,
        const Vec4f& color,
        std::shared_ptr<Font> font,
        float maxWidth = 0.0f,
        TextAlignment alignment = TextAlignment::Left);
    
    /**
     * @brief Create text vertices with rotation
     */
    static std::vector<UIVertex> createTextVerticesRotated(
        const std::string& text,
        const Vec2f& position,
        float fontSize,
        const Vec4f& color,
        std::shared_ptr<Font> font,
        float rotation,
        TextAlignment alignment = TextAlignment::Left);
    
    /**
     * @brief Create icon vertices
     */
    static std::vector<UIVertex> createIconVertices(
        const Vec2f& position,
        float size,
        std::shared_ptr<Font> font,
        uint32_t character);
    
    /**
     * @brief Render text to command buffer
     */
    static void renderText(CommandBuffer& cmdBuffer,
                          const std::string& text,
                          const Vec2f& position,
                          float fontSize,
                          const Vec4f& color,
                          std::shared_ptr<Font> font,
                          float maxWidth = 0.0f,
                          TextAlignment alignment = TextAlignment::Left);
};

} // namespace vgui
