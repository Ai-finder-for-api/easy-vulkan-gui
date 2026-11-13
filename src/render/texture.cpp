// Textures: BMP and TGA decoding, BMP writing, and texel fetch / nearest / bilinear sampling.
#include <evg/texture.hpp>
#include <evg/common/file.hpp>
#include "mem.hpp"

namespace evgx { namespace tex {
using namespace math;

namespace {

uint32_t rd16le(const uint8_t* p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8); }
uint32_t rd32le(const uint8_t* p) { return rd16le(p) | (rd16le(p + 2) << 16); }

uint32_t pack_argb(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

bool alloc_pixels(Texture& t, int w, int h, uint32_t fill) {
    t.pixels = alloc_arr<uint32_t>((size_t)w * (size_t)h);
    if (!t.pixels) return false;
    t.w = w;
    t.h = h;
    for (size_t i = 0; i < (size_t)w * h; i++) t.pixels[i] = fill;
    return true;
}

}  // namespace

bool create(Texture& t, int w, int h, uint32_t color) {
    free(t);
    if (w <= 0 || h <= 0 || w > 16384 || h > 16384) return false;
    return alloc_pixels(t, w, h, color);
}

void free(Texture& t) {
    mem_free(t.pixels);
    t.pixels = nullptr;
    t.w = 0;
    t.h = 0;
}

bool load_bmp_memory(Texture& t, const uint8_t* d, size_t n, const char** err) {
    free(t);
    if (n < 54 || d[0] != 'B' || d[1] != 'M') { *err = "not a BMP file"; return false; }
    size_t off = rd32le(d + 10);
    uint32_t hsz = rd32le(d + 14);
    if (hsz < 40) { *err = "unsupported BMP header"; return false; }
    int32_t W = (int32_t)rd32le(d + 18), H = (int32_t)rd32le(d + 22);
    uint32_t planes = rd16le(d + 26), bpp = rd16le(d + 28), comp = rd32le(d + 30);
    if (planes != 1 || W <= 0 || H == 0 || W > 16384 || H > 16384 || H < -16384) { *err = "bad BMP size"; return false; }
    if (bpp == 24 && comp == 0) {}
    else if (bpp == 32 && comp == 0) {}
    else if (bpp == 32 && comp == 3) {                               // BI_BITFIELDS: only the standard BGRA masks
        if (n < 66 || rd32le(d + 54) != 0x00FF0000u || rd32le(d + 58) != 0x0000FF00u || rd32le(d + 62) != 0x000000FFu) {
            *err = "unsupported BMP bit masks";
            return false;
        }
    } else { *err = "unsupported BMP format (need 24 or 32 bit, uncompressed)"; return false; }

    bool topdown = H < 0;
    int h = topdown ? -H : H;
    size_t stride = (((size_t)W * bpp / 8) + 3) & ~(size_t)3;     // rows are padded to 4 bytes
    if (off > n || stride * (size_t)h > n - off) { *err = "BMP data is truncated"; return false; }
    if (!alloc_pixels(t, W, h, 0xFF000000u)) { *err = "out of memory"; return false; }

    for (int y = 0; y < h; y++) {
        const uint8_t* row = d + off + (size_t)(topdown ? y : h - 1 - y) * stride;
        for (int x = 0; x < W; x++) {
            if (bpp == 24) {
                t.pixels[(size_t)y * W + x] = pack_argb(row[x * 3 + 2], row[x * 3 + 1], row[x * 3], 255);
            } else {
                const uint8_t* p = row + x * 4;
                uint8_t a = comp == 3 ? p[3] : 255;                 // BI_RGB leaves the fourth byte unused
                t.pixels[(size_t)y * W + x] = pack_argb(p[2], p[1], p[0], a);
            }
        }
    }
    return true;
}

bool load_tga_memory(Texture& t, const uint8_t* d, size_t n, const char** err) {
    free(t);
    if (n < 18) { *err = "not a TGA file"; return false; }
    size_t idlen = d[0];
    uint8_t cmapType = d[1], type = d[2], bpp = d[16], desc = d[17];
    int W = (int)rd16le(d + 12), H = (int)rd16le(d + 14);
    if (cmapType != 0) { *err = "color-mapped TGA is not supported"; return false; }
    bool rle = type == 10 || type == 11;
    bool gray = type == 3 || type == 11;
    if (!(type == 2 || type == 10 || gray)) { *err = "unsupported TGA image type"; return false; }
    if (gray ? bpp != 8 : (bpp != 24 && bpp != 32)) { *err = "unsupported TGA pixel size"; return false; }
    if (W <= 0 || H <= 0 || W > 16384 || H > 16384) { *err = "bad TGA size"; return false; }
    bool topLeft = (desc & 0x20) != 0;
    size_t bytes = bpp / 8;
    size_t pos = 18 + idlen;
    if (pos > n) { *err = "TGA header is truncated"; return false; }
    if (!alloc_pixels(t, W, H, 0xFF000000u)) { *err = "out of memory"; return false; }

    size_t total = (size_t)W * H;
    auto put = [&](size_t i, const uint8_t* p) {
        uint32_t c = gray ? pack_argb(p[0], p[0], p[0], 255)
                          : pack_argb(p[2], p[1], p[0], bpp == 32 ? p[3] : 255);
        int row = (int)(i / W), col = (int)(i % W);
        if (!topLeft) row = H - 1 - row;                             // bottom-left origin is the default
        t.pixels[(size_t)row * W + col] = c;
    };

    size_t i = 0;
    if (!rle) {
        if (pos + total * bytes > n) { free(t); *err = "TGA data is truncated"; return false; }
        for (; i < total; i++) put(i, d + pos + i * bytes);
        return true;
    }
    while (i < total) {                                              // RLE packets: header bit 7 = repeat, low 7 = count-1
        if (pos >= n) { free(t); *err = "TGA RLE data is truncated"; return false; }
        uint8_t hdr = d[pos++];
        size_t count = (size_t)(hdr & 0x7F) + 1;
        if (hdr & 0x80) {
            if (pos + bytes > n) { free(t); *err = "TGA RLE data is truncated"; return false; }
            for (size_t k = 0; k < count && i < total; k++) put(i++, d + pos);
            pos += bytes;
        } else {
            if (pos + count * bytes > n) { free(t); *err = "TGA RLE data is truncated"; return false; }
            for (size_t k = 0; k < count && i < total; k++) put(i++, d + pos + k * bytes);
            pos += count * bytes;
        }
    }
    return true;
}

bool load_bmp(Texture& t, const char* path, const char** err) {
    uint8_t* data = nullptr;
    size_t n = 0;
    if (!file::read_all(path, data, n)) { *err = "cannot read file"; return false; }
    bool ok = load_bmp_memory(t, data, n, err);
    file::free_buffer(data, n);
    return ok;
}

bool load_tga(Texture& t, const char* path, const char** err) {
    uint8_t* data = nullptr;
    size_t n = 0;
    if (!file::read_all(path, data, n)) { *err = "cannot read file"; return false; }
    bool ok = load_tga_memory(t, data, n, err);
    file::free_buffer(data, n);
    return ok;
}

bool save_bmp(const Texture& t, const char* path) {
    if (!t.valid()) return false;
    size_t pixels = (size_t)t.w * t.h * 4;
    size_t total = 54 + pixels;
    uint8_t* b = (uint8_t*)mem_alloc(total);
    if (!b) return false;
    auto w16 = [](uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); };
    auto w32 = [](uint8_t* p, uint32_t v) { for (int k = 0; k < 4; k++) p[k] = (uint8_t)(v >> (8 * k)); };
    b[0] = 'B'; b[1] = 'M';
    w32(b + 2, (uint32_t)total);
    w32(b + 10, 54);
    w32(b + 14, 40);
    w32(b + 18, (uint32_t)t.w);
    w32(b + 22, (uint32_t)(-t.h));                                    // negative height: top row first
    w16(b + 26, 1);
    w16(b + 28, 32);
    w32(b + 30, 0);                                                   // BI_RGB
    w32(b + 34, (uint32_t)pixels);
    for (int y = 0; y < t.h; y++)
        for (int x = 0; x < t.w; x++) {
            uint32_t c = t.pixels[(size_t)y * t.w + x];
            uint8_t* p = b + 54 + ((size_t)y * t.w + x) * 4;
            p[0] = (uint8_t)c; p[1] = (uint8_t)(c >> 8); p[2] = (uint8_t)(c >> 16); p[3] = (uint8_t)(c >> 24);
        }
    bool ok = file::write_all(path, b, total);
    mem_free(b);
    return ok;
}

uint32_t fetch(const Texture& t, int x, int y, Wrap wrap) {
    if (!t.valid()) return 0;
    if (wrap == Wrap::Repeat) {
        x %= t.w; if (x < 0) x += t.w;
        y %= t.h; if (y < 0) y += t.h;
    } else {
        x = x < 0 ? 0 : (x >= t.w ? t.w - 1 : x);
        y = y < 0 ? 0 : (y >= t.h ? t.h - 1 : y);
    }
    return t.pixels[(size_t)y * t.w + x];
}

namespace {
vec4<float> to_vec(uint32_t c) {
    return vec4<float>(((c >> 16) & 255) / 255.f, ((c >> 8) & 255) / 255.f, (c & 255) / 255.f, (c >> 24) / 255.f);
}
}

vec4<float> sample_nearest(const Texture& t, float u, float v, Wrap wrap) {
    if (!t.valid()) return vec4<float>(0.f, 0.f, 0.f, 0.f);
    int x = (int)floor(u * t.w), y = (int)floor(v * t.h);
    return to_vec(fetch(t, x, y, wrap));
}

vec4<float> sample_bilinear(const Texture& t, float u, float v, Wrap wrap) {
    if (!t.valid()) return vec4<float>(0.f, 0.f, 0.f, 0.f);
    float fx = u * t.w - 0.5f, fy = v * t.h - 0.5f;
    int x0 = (int)floor(fx), y0 = (int)floor(fy);
    float tx = fx - x0, ty = fy - y0;
    vec4<float> a = to_vec(fetch(t, x0, y0, wrap)), b = to_vec(fetch(t, x0 + 1, y0, wrap));
    vec4<float> c = to_vec(fetch(t, x0, y0 + 1, wrap)), d = to_vec(fetch(t, x0 + 1, y0 + 1, wrap));
    return mix(mix(a, b, tx), mix(c, d, tx), ty);
}

}}  // namespace evgx::tex
