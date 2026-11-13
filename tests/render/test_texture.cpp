// Textures: BMP and TGA decoding (built in memory, so no asset files are needed), BMP round trip, wrapping and sampling.
#include <string.h>
#include "../common/check.hpp"
#include <evg/texture.hpp>

using namespace evgx;

static void put16(uint8_t* p, uint32_t v) { p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); }
static void put32(uint8_t* p, uint32_t v) { for (int k = 0; k < 4; k++) p[k] = (uint8_t)(v >> (8 * k)); }

int main() {
    const char* err = nullptr;

    // create, fill, and both wrap modes
    tex::Texture t;
    CHECK(tex::create(t, 2, 2, 0xFF102030u));
    CHECK(t.valid() && t.w == 2 && t.h == 2 && t.pixels[3] == 0xFF102030u);
    t.pixels[0] = 0xFF0000FFu;                       // blue at (0,0)
    t.pixels[1] = 0xFFFFFFFFu;                       // white at (1,0)
    CHECK(tex::fetch(t, 0, 0) == 0xFF0000FFu);
    CHECK(tex::fetch(t, 2, 0) == 0xFF0000FFu);       // repeat wraps 2 -> 0
    CHECK(tex::fetch(t, -1, 0) == 0xFFFFFFFFu);      // repeat wraps -1 -> 1
    CHECK(tex::fetch(t, -5, 7, tex::Wrap::Clamp) == 0xFF102030u);   // clamp: (-5,7) -> (0,1), the fill color
    CHECK(tex::fetch(t, 9, 0, tex::Wrap::Clamp) == 0xFFFFFFFFu);    // clamp: x 9 -> 1

    // 24-bit bottom-up BMP, 2x2, with 2 padding bytes per row
    uint8_t bmp[70] = {};
    bmp[0] = 'B'; bmp[1] = 'M';
    put32(bmp + 2, sizeof bmp); put32(bmp + 10, 54); put32(bmp + 14, 40);
    put32(bmp + 18, 2); put32(bmp + 22, 2);          // positive height: rows stored bottom first
    put16(bmp + 26, 1); put16(bmp + 28, 24); put32(bmp + 30, 0);
    uint8_t* row0 = bmp + 54;                        // bottom row: red, green (BGR order)
    row0[0] = 0; row0[1] = 0; row0[2] = 255;
    row0[3] = 0; row0[4] = 255; row0[5] = 0;
    uint8_t* row1 = bmp + 62;                        // top row: blue, white
    row1[0] = 255; row1[1] = 0; row1[2] = 0;
    row1[3] = 255; row1[4] = 255; row1[5] = 255;
    tex::Texture b;
    CHECK(tex::load_bmp_memory(b, bmp, sizeof bmp, &err));
    CHECK(b.w == 2 && b.h == 2);
    CHECK(b.pixels[0] == 0xFF0000FFu && b.pixels[1] == 0xFFFFFFFFu);   // top row: blue, white
    CHECK(b.pixels[2] == 0xFFFF0000u && b.pixels[3] == 0xFF00FF00u);   // bottom row: red, green

    // BMP round trip through a file
    CHECK(tex::save_bmp(b, "tex_roundtrip.bmp"));
    tex::Texture back;
    CHECK(tex::load_bmp(back, "tex_roundtrip.bmp", &err));
    CHECK(back.w == 2 && back.h == 2);
    bool same = true;
    for (int i = 0; i < 4; i++) if (back.pixels[i] != b.pixels[i]) same = false;
    CHECK(same);
    tex::free(back);

    // 32-bit TGA, uncompressed, bottom-left origin, 2x1: BGRA on disk
    uint8_t tga32[18 + 8] = {};
    tga32[2] = 2; put16(tga32 + 12, 2); put16(tga32 + 14, 1); tga32[16] = 32; tga32[17] = 0x08;
    tga32[18] = 10; tga32[19] = 20; tga32[20] = 30; tga32[21] = 255;     // pixel 0
    tga32[22] = 0; tga32[23] = 0; tga32[24] = 255; tga32[25] = 128;      // pixel 1
    tex::Texture c;
    CHECK(tex::load_tga_memory(c, tga32, sizeof tga32, &err));
    CHECK(c.w == 2 && c.h == 1);
    CHECK(c.pixels[0] == 0xFF1E140Au);               // rgb(30,20,10)
    CHECK(c.pixels[1] == 0x80FF0000u);               // rgb(255,0,0) with alpha 128

    // RLE 24-bit TGA, 4x1: repeat packet of two (3,2,1), raw packet of (6,5,4) and (9,8,7)
    uint8_t rle[18 + 2 + 3 + 1 + 6] = {};
    rle[2] = 10; put16(rle + 12, 4); put16(rle + 14, 1); rle[16] = 24; rle[17] = 0x20;
    size_t p = 18;
    rle[p++] = 0x81; rle[p++] = 1; rle[p++] = 2; rle[p++] = 3;          // repeat: 2 pixels, B=1 G=2 R=3
    rle[p++] = 0x01; rle[p++] = 4; rle[p++] = 5; rle[p++] = 6;          // raw: 2 pixels follow
    rle[p++] = 7; rle[p++] = 8; rle[p++] = 9;
    tex::Texture r;
    CHECK(tex::load_tga_memory(r, rle, p, &err));
    CHECK(r.w == 4 && r.h == 1);
    CHECK(r.pixels[0] == 0xFF030201u && r.pixels[1] == 0xFF030201u);
    CHECK(r.pixels[2] == 0xFF060504u && r.pixels[3] == 0xFF090807u);

    // grayscale TGA, 8 bit, 2x1
    uint8_t gray[18 + 2] = {};
    gray[2] = 3; put16(gray + 12, 2); put16(gray + 14, 1); gray[16] = 8; gray[17] = 0x20;
    gray[18] = 10; gray[19] = 200;
    tex::Texture g;
    CHECK(tex::load_tga_memory(g, gray, sizeof gray, &err));
    CHECK(g.pixels[0] == 0xFF0A0A0Au && g.pixels[1] == 0xFFC8C8C8u);

    // malformed input is rejected with a message and no leak
    const uint8_t junk[] = "definitely not an image";
    tex::Texture bad;
    CHECK(!tex::load_bmp_memory(bad, junk, sizeof junk, &err) && err != nullptr && strstr(err, "BMP"));
    CHECK(!bad.valid());
    CHECK(!tex::load_bmp_memory(bad, bmp, 60, &err));                      // truncated pixel data
    CHECK(!tex::load_tga_memory(bad, tga32, 10, &err));                    // truncated header
    uint8_t cmap[18] = {};
    cmap[1] = 1; cmap[2] = 1; cmap[16] = 8;
    CHECK(!tex::load_tga_memory(bad, cmap, sizeof cmap, &err) && strstr(err, "color-mapped"));
    uint8_t wrong[18] = {};
    wrong[2] = 99; put16(wrong + 12, 1); put16(wrong + 14, 1); wrong[16] = 32;
    CHECK(!tex::load_tga_memory(bad, wrong, sizeof wrong, &err));
    CHECK(!tex::load_bmp(bad, "no/such/file.bmp", &err));

    // sampling: nearest picks the texel; bilinear blends two neighbours in the middle
    tex::Texture s;
    CHECK(tex::create(s, 2, 1, 0xFF000000u));
    s.pixels[1] = 0xFFFFFFFFu;
    CHECK(tex::sample_nearest(s, 0.25f, 0.5f, tex::Wrap::Clamp).x < 0.01f);
    CHECK(tex::sample_nearest(s, 0.75f, 0.5f, tex::Wrap::Clamp).x > 0.99f);
    math::vec4<float> mid = tex::sample_bilinear(s, 0.5f, 0.5f, tex::Wrap::Clamp);
    CHECK(mid.x > 0.45f && mid.x < 0.55f && mid.w > 0.99f);

    tex::free(t);
    tex::free(b);
    tex::free(c);
    tex::free(r);
    tex::free(g);
    tex::free(s);
    return evgtest::finish("render/texture");
}
