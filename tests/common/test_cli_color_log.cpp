// Argument parsing, colors, logging sink, file round trip, and the stopwatch.
#include <string.h>
#include <math.h>
#include "../common/check.hpp"
#include <evg/common/args.hpp>
#include <evg/common/color.hpp>
#include <evg/common/log.hpp>
#include <evg/common/file.hpp>
#include <evg/common/timer.hpp>

using namespace evgx;
using log::Level;

static int g_logged = 0;
static Level g_lastLevel = Level::Off;
static void capture(void*, log::Level level, const char* msg, size_t n) {
    g_logged++;
    g_lastLevel = level;
    CHECK(n == strlen(msg));
}

int main() {
    // arguments: flags, key=value, key value, short flags, positionals, and "--"
    const char* argv[] = {"prog", "--verbose", "--size=640", "--name", "scene", "-v", "model.glb", "--", "--not-an-option"};
    cli::Args args(9, argv);
    CHECK(args.has("verbose") && args.has("v") && !args.has("quiet"));
    CHECK(args.get_int("size", 0) == 640);
    CHECK(strcmp(args.get("name", "none"), "scene") == 0);
    CHECK(strcmp(args.get("missing", "none"), "none") == 0);
    CHECK(args.get_double("size", 1.5) == 640.0);
    CHECK(args.positional_count() == 2);
    CHECK(strcmp(args.positional(0), "model.glb") == 0 && strcmp(args.positional(1), "--not-an-option") == 0);
    CHECK(args.positional(5) == nullptr);

    // colors: packing, channel extraction, clamped conversion, sRGB round trip, hue wheel, lerp
    uint32_t c = color::pack(10, 20, 30, 40);
    CHECK(color::red(c) == 10 && color::green(c) == 20 && color::blue(c) == 30 && color::alpha(c) == 40);
    CHECK(color::to_u8(2.f) == 255 && color::to_u8(-1.f) == 0 && color::to_u8(0.5f) == 128);
    CHECK(fabsf(color::linear_to_srgb(color::srgb_to_linear(0.3f)) - 0.3f) < 1e-4f);
    CHECK(fabsf(color::srgb_to_linear(1.f) - 1.f) < 1e-5f);
    math::vec3<float> red = color::hsv_to_rgb(0.f, 1.f, 1.f);
    CHECK(fabsf(red.x - 1.f) < 1e-5f && red.y < 1e-5f && red.z < 1e-5f);
    math::vec3<float> green = color::hsv_to_rgb(120.f, 1.f, 1.f);
    CHECK(fabsf(green.y - 1.f) < 1e-5f && green.x < 1e-5f);
    math::vec3<float> wrapped = color::hsv_to_rgb(-240.f, 1.f, 1.f);   // -240 is the same hue as 120
    CHECK(fabsf(wrapped.y - 1.f) < 1e-5f);
    CHECK(color::lerp(0xFF000000u, 0xFFFFFFFFu, 0.5f) == 0xFF808080u || color::lerp(0xFF000000u, 0xFFFFFFFFu, 0.5f) == 0xFF7F7F7Fu);
    CHECK(color::lerp(0xFF102030u, 0xFF102030u, 0.7f) == 0xFF102030u);

    // logging: nothing without a sink; levels filter; the sink sees the message
    log::set_sink(nullptr);
    log::info("dropped: no sink");
    CHECK(g_logged == 0);
    log::set_sink(capture, nullptr, Level::Warn);
    log::info("below the minimum");
    CHECK(g_logged == 0);
    log::warn("careful");
    CHECK(g_logged == 1 && g_lastLevel == Level::Warn);
    log::error("bad");
    CHECK(g_logged == 2 && g_lastLevel == Level::Error);
    log::set_sink(nullptr);

    // file round trip through evg's allocator
    const char path[] = "common_roundtrip.bin";
    unsigned char payload[] = {1, 2, 3, 250, 0, 9};
    CHECK(file::write_all(path, payload, sizeof payload));
    CHECK(file::size(path) == (long long)sizeof payload);
    uint8_t* data = nullptr;
    size_t n = 0;
    CHECK(file::read_all(path, data, n));
    CHECK(n == sizeof payload && data && memcmp(data, payload, sizeof payload) == 0);
    file::free_buffer(data, n);
    CHECK(file::size("no/such/file.bin") == -1);
    uint8_t* none = nullptr;
    size_t nn = 0;
    CHECK(!file::read_all("no/such/file.bin", none, nn) && none == nullptr);

    // stopwatch and scope timer move forward
    timer::Stopwatch sw;
    evg.sleep(5);
    CHECK(sw.millis() >= 4.0);
    double total = 0;
    {
        timer::Scoped s(total);
        evg.sleep(5);
    }
    CHECK(total >= 0.004);

    return evgtest::finish("common/cli_color_log_file");
}
