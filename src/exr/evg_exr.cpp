// Optional module: OpenEXR (.exr) reader for evg.import_lighting.
// The only place evg touches a third-party library. The core library stays dependency-free.
#include <ImfRgbaFile.h>
#include <ImfArray.h>
#include <ImathBox.h>
#include <exception>
#include <stdlib.h>

namespace evgx {
bool evg_exr_read(const char* path, float** rgb, int* w, int* h, const char** err) {
    *rgb = nullptr; *w = *h = 0; *err = nullptr;
    try {
        Imf::RgbaInputFile f(path);
        Imath::Box2i dw = f.dataWindow();
        int W = dw.max.x - dw.min.x + 1, H = dw.max.y - dw.min.y + 1;
        if (W <= 0 || H <= 0 || (long long)W * H > 64LL * 1024 * 1024) { *err = "EXR size unsupported"; return false; }

        // one RGBA half-float per pixel, row-major, origin at the data window's top-left
        Imf::Array2D<Imf::Rgba> px(H, W);
        f.setFrameBuffer(&px[0][0] - dw.min.x - (size_t)dw.min.y * W, 1, (size_t)W);
        f.readPixels(dw.min.y, dw.max.y);

        float* out = (float*)malloc(sizeof(float) * (size_t)W * H * 3);
        if (!out) { *err = "out of memory"; return false; }
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++) {
                const Imf::Rgba& p = px[y][x];
                float* o = out + ((size_t)y * W + x) * 3;
                o[0] = (float)p.r; o[1] = (float)p.g; o[2] = (float)p.b;
            }
        *rgb = out; *w = W; *h = H;
        return true;
    } catch (const std::exception&) {
        *err = "cannot read EXR (missing, corrupt, or not an RGBA EXR)";
        return false;
    }
}
}  // namespace evgx

