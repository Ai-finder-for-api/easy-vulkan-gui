// The global `evg` object and its small utility methods: timing, sleep, error text, GPU name.
#include "state.hpp"
#include "platform.hpp"

evgx::Api evg;

namespace evgx {
namespace vkp { const char* gpu_name(); }

void Api::sleep(int ms) { plat::sleep_ms(ms); }
double Api::time() const { return plat::time_seconds(); }
void Api::seterror(const char* message) { st::copy(st::g_err, message ? message : "", sizeof st::g_err); }
const char* Api::error() const { return st::g_err; }

const char* Api::gpu() const {
    if (st::g_gpu[0]) return st::g_gpu;
    const char* v = vkp::gpu_name();
    return v ? v : "";
}

}  // namespace evgx
