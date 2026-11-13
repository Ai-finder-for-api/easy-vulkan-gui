// Codecs: zlib/DEFLATE framing and checksums, PNG chunk handling and scanline filters (on PNG files built in memory),
// and two real PNG files from the repository's docs folder (dynamic-Huffman data, so a wrong decode fails Adler-32).
#include <string.h>
#include "../common/check.hpp"
#include <evg/texture.hpp>
#include <evg/common/inflate.hpp>

#ifndef EVG_ROOT_DIR
#define EVG_ROOT_DIR "."
#endif

using namespace evgx;

static void be32(uint8_t* p, uint32_t v) { p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }

// appends one PNG chunk (length, type, data, CRC) to out; returns the new size
static size_t chunk(uint8_t* out, size_t at, const char* type, const uint8_t* data, uint32_t len) {
    be32(out + at, len);
    memcpy(out + at + 4, type, 4);
    if (len) memcpy(out + at + 8, data, len);
    be32(out + at + 8 + len, inflate::crc32_ieee(out + at + 4, 4 + len));
    return at + 12 + len;
}

int main() {
    const char* err = nullptr;

    // ---- checksums against known values ----
    CHECK(inflate::adler32((const uint8_t*)"Wikipedia", 9) == 0x11E60398u);
    CHECK(inflate::crc32_ieee((const uint8_t*)"123456789", 9) == 0xCBF43926u);

    // ---- zlib with one stored block: header 78 01, block header 01, LEN/NLEN, data, Adler-32 ----
    const uint8_t hello[] = {0x78, 0x01, 0x01, 0x05, 0x00, 0xFA, 0xFF, 'h', 'e', 'l', 'l', 'o',
                             0x06, 0x2C, 0x02, 0x15};
    uint8_t* out = nullptr;
    size_t outn = 0;
    CHECK(inflate::zlib_decompress(hello, sizeof hello, out, outn, &err));
    CHECK(outn == 5 && out && memcmp(out, "hello", 5) == 0);
    inflate::free_buffer(out);

    // flipped checksum byte: must be caught
    uint8_t badSum[sizeof hello];
    memcpy(badSum, hello, sizeof hello);
    badSum[sizeof hello - 1] ^= 1;
    out = nullptr;
    CHECK(!inflate::zlib_decompress(badSum, sizeof hello, out, outn, &err) && out == nullptr && strstr(err, "Adler"));

    // wrong header, short input, and truncated block are rejected without crashing
    uint8_t badHdr[sizeof hello];
    memcpy(badHdr, hello, sizeof hello);
    badHdr[0] = 0x79;
    CHECK(!inflate::zlib_decompress(badHdr, sizeof hello, out, outn, &err));
    CHECK(!inflate::zlib_decompress(hello, 4, out, outn, &err));
    CHECK(!inflate::zlib_decompress(hello, 9, out, outn, &err));

    // raw DEFLATE: fixed-Huffman literal block for "a" (0x4B 0x04 0x00)
    const uint8_t fixedA[] = {0x4B, 0x04, 0x00};
    CHECK(inflate::deflate_decompress(fixedA, sizeof fixedA, out, outn, &err));
    CHECK(outn == 1 && out[0] == 'a');
    inflate::free_buffer(out);

    // ---- synthetic PNG: 2x2 RGB, one row per filter type 1 (sub) and 2 (up) ----
    uint8_t raw[14] = {1, 10, 20, 30, 30, 30, 30,    // row 0, sub-filtered: (10,20,30) (40,50,60)
                       2, 60, 60, 60, 60, 60, 60};   // row 1, up-filtered:  (70,80,90) (100,110,120)
    uint8_t z[2 + 5 + 14 + 4];
    z[0] = 0x78; z[1] = 0x01;
    z[2] = 0x01;                                     // final stored block
    z[3] = 14; z[4] = 0; z[5] = 0xF1; z[6] = 0xFF;   // LEN 14, NLEN
    memcpy(z + 7, raw, 14);
    be32(z + 21, inflate::adler32(raw, 14));

    uint8_t ihdr[13] = {0, 0, 0, 2, 0, 0, 0, 2, 8, 2, 0, 0, 0};
    uint8_t png[8 + 12 + 13 + 12 + sizeof z + 12];
    memcpy(png, "\x89PNG\r\n\x1a\n", 8);
    size_t at = 8;
    at = chunk(png, at, "IHDR", ihdr, 13);
    at = chunk(png, at, "IDAT", z, sizeof z);
    at = chunk(png, at, "IEND", nullptr, 0);

    tex::Texture t;
    CHECK(tex::load_png_memory(t, png, at, &err));
    CHECK(t.w == 2 && t.h == 2);
    if (t.valid()) {
        CHECK(t.pixels[0] == 0xFF0A141Eu);           // (10,20,30)
        CHECK(t.pixels[1] == 0xFF28323Cu);           // (40,50,60)
        CHECK(t.pixels[2] == 0xFF46505Au);           // (70,80,90)
        CHECK(t.pixels[3] == 0xFF646E78u);           // (100,110,120)
    }

    // a flipped byte inside a chunk fails its CRC
    uint8_t corrupt[8 + 12 + 13 + 12 + sizeof z + 12];
    memcpy(corrupt, png, at);
    corrupt[8 + 12 + 13 + 12 + 10] ^= 0x40;
    CHECK(!tex::load_png_memory(t, corrupt, at, &err) && strstr(err, "CRC"));
    CHECK(!t.valid());

    // not a PNG
    CHECK(!tex::load_png_memory(t, (const uint8_t*)"GIF89a....", 10, &err) && strstr(err, "PNG"));

    // ---- real files: the repository's screenshots (zlib dynamic-Huffman data, many filter types) ----
    char path[512];
    snprintf(path, sizeof path, "%s/docs/scene.png", EVG_ROOT_DIR);
    tex::Texture scene;
    CHECK(tex::load_png(scene, path, &err));
    CHECK(scene.valid() && scene.w > 100 && scene.h > 100);
    if (scene.valid()) {
        // a rendered scene has a background; the first pixel is some opaque color
        CHECK((scene.pixels[0] >> 24) == 255);
    }

    snprintf(path, sizeof path, "%s/docs/cesium_milk_truck.png", EVG_ROOT_DIR);
    tex::Texture truck;
    CHECK(tex::load_png(truck, path, &err));
    CHECK(truck.valid() && truck.w > 100 && truck.h > 100);

    // a bad path reports an error
    tex::Texture none;
    CHECK(!tex::load_png(none, "no/such/file.png", &err));

    tex::free(t);
    tex::free(scene);
    tex::free(truck);
    return evgtest::finish("render/codec");
}
