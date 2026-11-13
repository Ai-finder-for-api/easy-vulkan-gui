// File helpers implementation on top of the platform layer and evg's allocator.
#include <evg/common/file.hpp>
#include "platform.hpp"
#include "mem.hpp"

namespace evgx { namespace file {

long long size(const char* path) { return plat::file_size(path); }

bool read_all(const char* path, uint8_t*& data, size_t& n) {
    data = nullptr;
    n = 0;
    long long sz = plat::file_size(path);
    if (sz < 0) return false;
    uint8_t* buf = (uint8_t*)mem_alloc((size_t)sz ? (size_t)sz : 1);
    if (!buf) return false;
    if (sz > 0 && !plat::file_read(path, buf, (size_t)sz)) {
        mem_free(buf);
        return false;
    }
    data = buf;
    n = (size_t)sz;
    return true;
}

bool write_all(const char* path, const void* data, size_t n) {
    return plat::file_write(path, data, n);
}

void free_buffer(uint8_t* data, size_t) { mem_free(data); }

}}  // namespace evgx::file
