// Textures: CPU-side 8-bit images with BMP and TGA loaders and nearest/bilinear sampling. Pixels are 0xAARRGGBB
// (the same layout as the software framebuffer and evg::color::pack). PNG and JPEG need decoders and are not included.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <evg/math.hpp>

namespace evgx { namespace tex {

struct Texture {
    int w = 0, h = 0;
    uint32_t* pixels = nullptr;      // row-major, top row first
    bool valid() const { return pixels && w > 0 && h > 0; }
};

enum class Wrap : int { Repeat = 0, Clamp = 1 };

// creates a w x h texture filled with one color (0xAARRGGBB); free with free()
bool create(Texture& t, int w, int h, uint32_t color = 0xFFFFFFFFu);
void free(Texture& t);

// loaders: uncompressed 24/32-bit BMP (BI_RGB, and 32-bit BI_BITFIELDS with BGRA masks); TGA types 2, 10 (RLE)
// truecolor 24/32-bit, and 3, 11 grayscale 8-bit. Anything else fails with evg.error() set by the caller.
bool load_bmp_memory(Texture& t, const uint8_t* data, size_t n, const char** err);
bool load_tga_memory(Texture& t, const uint8_t* data, size_t n, const char** err);
bool load_bmp(Texture& t, const char* path, const char** err);
bool load_tga(Texture& t, const char* path, const char** err);

// writes the texture as an uncompressed 32-bit BMP (round trip for tools and tests)
bool save_bmp(const Texture& t, const char* path);

// color at texel (x, y) with the given wrap mode (out-of-range coordinates wrap or clamp)
uint32_t fetch(const Texture& t, int x, int y, Wrap wrap = Wrap::Repeat);

// nearest-texel sample at texture coordinates (u, v) in [0,1]; returns RGBA in [0,1]
math::vec4<float> sample_nearest(const Texture& t, float u, float v, Wrap wrap = Wrap::Repeat);

// bilinear sample at (u, v); the four neighbours are blended in linear 0..1 space
math::vec4<float> sample_bilinear(const Texture& t, float u, float v, Wrap wrap = Wrap::Repeat);

}}  // namespace evgx::tex
