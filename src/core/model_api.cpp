// Model loading API: loadglb (file), loadglb_memory (bytes), freemodel. Keeps the model table.
#include "state.hpp"
#include "platform.hpp"
#include "mem.hpp"
#include "glb.hpp"

namespace evgx {
using namespace st;

Model* Api::loadglb_memory(const void* data, size_t size) {
    const char* err = nullptr;
    Model* m = glb_parse((const uint8_t*)data, size, &err);
    if (!m) { fail(err ? err : "GLB load failed"); return nullptr; }
    for (int i = 0; i < kMaxModels; i++)
        if (!g_models[i]) { g_models[i] = m; g_err[0] = 0; return m; }
    model_free(m);
    fail("too many models");
    return nullptr;
}

Model* Api::loadglb(const char* path) {
    long long sz = plat::file_size(path);
    if (sz < 0) { fail("cannot open file"); return nullptr; }
    unsigned char* buf = (unsigned char*)mem_alloc((size_t)sz + 1);
    if (!buf) { fail("out of memory"); return nullptr; }
    Model* m = nullptr;
    if (!plat::file_read(path, buf, (size_t)sz)) fail("cannot read file");
    else m = loadglb_memory(buf, (size_t)sz);
    mem_free(buf);
    return m;
}

void Api::freemodel(Model* m) {
    if (!m) return;
    for (int i = 0; i < kMaxModels; i++)
        if (g_models[i] == m) { g_models[i] = nullptr; model_free(m); return; }
}

}  // namespace evgx
