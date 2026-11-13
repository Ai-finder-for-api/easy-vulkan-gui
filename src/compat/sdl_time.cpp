// SDL2 timing: ticks, performance counter and frequency, all counted from SDL_Init (or the first call).
#include "sdl_internal.hpp"

namespace evgx { namespace sdl {
namespace {
double g_t0 = -1.0;
}

double since_start() {
    if (g_t0 < 0) g_t0 = evg.time();
    return evg.time() - g_t0;
}

}}  // namespace evgx::sdl

extern "C" {
Uint32 SDL_GetTicks(void) { return (Uint32)(evgx::sdl::since_start() * 1000.0); }
Uint64 SDL_GetPerformanceFrequency(void) { return 1000000000ull; }                    // nanoseconds
Uint64 SDL_GetPerformanceCounter(void) { return (Uint64)(evgx::sdl::since_start() * 1e9) + 1; }
}
