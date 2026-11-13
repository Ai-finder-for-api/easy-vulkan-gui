// Api::screenshot: writes the last software frame as a 24-bit BMP (bottom-up rows, BGR order).
#include "state.hpp"
#include "platform.hpp"
#include "mem.hpp"

namespace evgx {
using namespace st;

bool Api::screenshot(Window* w, const char* path) {
    if (!w || !w->color) return fail("no frame to capture");
    int W = w->fbw, H = w->fbh;
    size_t row = ((size_t)W * 3 + 3) & ~(size_t)3;          // rows padded to 4 bytes
    size_t total = 54 + row * H;
    unsigned char* b = (unsigned char*)mem_alloc(total);
    if (!b) return fail("out of memory");
    b[0] = 'B'; b[1] = 'M';
    put32(b + 2, (uint32_t)total);
    put32(b + 10, 54);                                      // pixel data offset
    put32(b + 14, 40);                                      // BITMAPINFOHEADER size
    put32(b + 18, (uint32_t)W);
    put32(b + 22, (uint32_t)H);
    b[26] = 1; b[28] = 24;                                  // 1 plane, 24 bits per pixel
    put32(b + 34, (uint32_t)(row * H));
    for (int y = 0; y < H; y++) {
        unsigned char* o = b + 54 + row * (size_t)(H - 1 - y);
        for (int x = 0; x < W; x++) {
            uint32_t p = w->color[(size_t)y * W + x];
            o[x * 3] = (unsigned char)p;
            o[x * 3 + 1] = (unsigned char)(p >> 8);
            o[x * 3 + 2] = (unsigned char)(p >> 16);
        }
    }
    bool ok = plat::file_write(path, b, total);
    mem_free(b);
    return ok ? true : fail("cannot write screenshot");
}

}  // namespace evgx
