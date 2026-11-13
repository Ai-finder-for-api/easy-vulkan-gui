// PNG decoding: chunk parsing with CRC checks, zlib inflate of the image data, scanline unfiltering, and conversion
// to 0xAARRGGBB. Supports 8 and 16 bit gray, gray+alpha, RGB and RGBA, and 8 bit palette images with optional tRNS.
// Not supported (reported as errors): interlaced (Adam7) images and bit depths 1, 2 and 4.
#include <evg/texture.hpp>
#include <evg/common/file.hpp>
#include <evg/common/inflate.hpp>
#include "mem.hpp"

namespace evgx { namespace tex {

namespace {

// byte comparison without the C library: 0 when equal
int memcmp_(const void* a, const void* b, size_t n) {
    const uint8_t* x = (const uint8_t*)a;
    const uint8_t* y = (const uint8_t*)b;
    for (size_t i = 0; i < n; i++) if (x[i] != y[i]) return (int)x[i] - (int)y[i];
    return 0;
}

uint32_t be32(const uint8_t* p) { return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3]; }

uint8_t paeth(uint8_t a, uint8_t b, uint8_t c) {
    int p = (int)a + (int)b - (int)c;
    int pa = p > (int)a ? p - (int)a : (int)a - p;
    int pb = p > (int)b ? p - (int)b : (int)b - p;
    int pc = p > (int)c ? p - (int)c : (int)c - p;
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

}  // namespace

bool load_png_memory(Texture& t, const uint8_t* d, size_t n, const char** err) {
    free(t);
    static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    if (n < 8 || memcmp_(d, sig, 8) != 0) { *err = "not a PNG file"; return false; }

    // ---- chunks: each one is length, type, data, CRC over type and data ----
    size_t pos = 8;
    bool haveHeader = false;
    uint32_t W = 0, H = 0;
    uint8_t depth = 0, ctype = 0, interlace = 0;
    const uint8_t* palette = nullptr;
    uint32_t paletteN = 0;
    const uint8_t* trns = nullptr;
    uint32_t trnsN = 0;
    size_t idatBytes = 0;
    while (pos + 12 <= n) {
        uint32_t len = be32(d + pos);
        const uint8_t* type = d + pos + 4;
        if (len > n - pos - 12) { *err = "PNG chunk is truncated"; return false; }
        const uint8_t* data = d + pos + 8;
        uint32_t want = be32(data + len);
        if (inflate::crc32_ieee(type, len + 4) != want) { *err = "PNG chunk CRC mismatch"; return false; }

        if (memcmp_(type, "IHDR", 4) == 0) {
            if (len != 13) { *err = "bad PNG header chunk"; return false; }
            W = be32(data);
            H = be32(data + 4);
            depth = data[8];
            ctype = data[9];
            interlace = data[12];
            if (data[10] != 0 || data[11] != 0) { *err = "unknown PNG compression or filter method"; return false; }
            haveHeader = true;
        } else if (memcmp_(type, "PLTE", 4) == 0) {
            if (len % 3 || len == 0 || len > 3 * 256) { *err = "bad PNG palette"; return false; }
            palette = data;
            paletteN = len / 3;
        } else if (memcmp_(type, "tRNS", 4) == 0) {
            trns = data;
            trnsN = len;
        } else if (memcmp_(type, "IDAT", 4) == 0) {
            idatBytes += len;
        } else if (memcmp_(type, "IEND", 4) == 0) {
            break;
        }
        pos += 12 + len;
    }
    if (!haveHeader) { *err = "PNG has no header chunk"; return false; }
    if (interlace != 0) { *err = "interlaced PNG is not supported"; return false; }
    if (W == 0 || H == 0 || W > 16384 || H > 16384) { *err = "bad PNG size"; return false; }

    // channels per pixel and the bytes each pixel takes in a scanline
    int channels;
    if (ctype == 0) channels = 1;
    else if (ctype == 2) channels = 3;
    else if (ctype == 3) channels = 1;
    else if (ctype == 4) channels = 2;
    else if (ctype == 6) channels = 4;
    else { *err = "unknown PNG color type"; return false; }
    if (ctype == 3) {
        if (depth != 8) { *err = "palette PNG bit depths other than 8 are not supported"; return false; }
        if (!palette) { *err = "palette PNG has no PLTE chunk"; return false; }
    } else if (depth != 8 && depth != 16) {
        *err = "PNG bit depths 1, 2 and 4 are not supported";
        return false;
    }
    size_t sampleBytes = depth / 8;
    size_t pixBytes = (size_t)channels * sampleBytes;          // filter stride: bytes per complete pixel
    size_t rowBytes = (size_t)W * pixBytes;
    size_t expect = (size_t)H * (rowBytes + 1);
    if (idatBytes == 0) { *err = "PNG has no image data"; return false; }

    // ---- join the IDAT chunks and inflate them ----
    uint8_t* zin = (uint8_t*)mem_alloc(idatBytes);
    if (!zin) { *err = "out of memory"; return false; }
    size_t at = 0;
    pos = 8;
    while (pos + 12 <= n) {
        uint32_t len = be32(d + pos);
        if (memcmp_(d + pos + 4, "IDAT", 4) == 0) {
            for (uint32_t i = 0; i < len; i++) zin[at++] = d[pos + 8 + i];
        }
        if (memcmp_(d + pos + 4, "IEND", 4) == 0) break;
        pos += 12 + len;
    }
    uint8_t* raw = nullptr;
    size_t rawn = 0;
    bool ok = inflate::zlib_decompress(zin, idatBytes, raw, rawn, err);
    mem_free(zin);
    if (!ok) return false;
    if (rawn < expect) { inflate::free_buffer(raw); *err = "PNG image data is too short"; return false; }

    // ---- undo the per-row filters into a plain pixel buffer ----
    uint8_t* px = (uint8_t*)mem_alloc(H * rowBytes);
    if (!px) { inflate::free_buffer(raw); *err = "out of memory"; return false; }
    for (uint32_t y = 0; y < H; y++) {
        uint8_t filter = raw[(size_t)y * (rowBytes + 1)];
        const uint8_t* src = raw + (size_t)y * (rowBytes + 1) + 1;
        uint8_t* cur = px + (size_t)y * rowBytes;
        const uint8_t* prev = y ? px + (size_t)(y - 1) * rowBytes : nullptr;
        for (size_t i = 0; i < rowBytes; i++) {
            uint8_t left = i >= pixBytes ? cur[i - pixBytes] : 0;
            uint8_t up = prev ? prev[i] : 0;
            uint8_t upLeft = (prev && i >= pixBytes) ? prev[i - pixBytes] : 0;
            uint8_t v;
            switch (filter) {
                case 0: v = src[i]; break;
                case 1: v = (uint8_t)(src[i] + left); break;
                case 2: v = (uint8_t)(src[i] + up); break;
                case 3: v = (uint8_t)(src[i] + (uint8_t)(((unsigned)left + up) / 2)); break;
                case 4: v = (uint8_t)(src[i] + paeth(left, up, upLeft)); break;
                default:
                    mem_free(px);
                    inflate::free_buffer(raw);
                    *err = "unknown PNG filter type";
                    return false;
            }
            cur[i] = v;
        }
    }
    inflate::free_buffer(raw);

    // ---- convert to 0xAARRGGBB (16-bit samples keep their high byte) ----
    if (!create(t, (int)W, (int)H, 0xFF000000u)) { mem_free(px); *err = "out of memory"; return false; }
    for (uint32_t y = 0; y < H; y++)
        for (uint32_t x = 0; x < W; x++) {
            const uint8_t* p = px + (size_t)y * rowBytes + (size_t)x * pixBytes;
            uint8_t r, g, b, a = 255;
            if (ctype == 3) {
                uint8_t idx = p[0];
                if (idx >= paletteN) { r = g = b = 0; }
                else { r = palette[idx * 3]; g = palette[idx * 3 + 1]; b = palette[idx * 3 + 2]; }
                if (trns && idx < trnsN) a = trns[idx];
            } else if (ctype == 0 || ctype == 4) {
                r = g = b = p[0];
                if (ctype == 4) a = p[sampleBytes];
            } else {
                r = p[0];
                g = p[sampleBytes];
                b = p[2 * sampleBytes];
                if (ctype == 6) a = p[3 * sampleBytes];
            }
            t.pixels[(size_t)y * W + x] = ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        }
    mem_free(px);
    return true;
}

bool load_png(Texture& t, const char* path, const char** err) {
    uint8_t* data = nullptr;
    size_t n = 0;
    if (!file::read_all(path, data, n)) { *err = "cannot read file"; return false; }
    bool ok = load_png_memory(t, data, n, err);
    file::free_buffer(data, n);
    return ok;
}

}}  // namespace evgx::tex
