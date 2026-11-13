#pragma once
#include <stddef.h>
#include "platform.hpp"
namespace evgx {
inline void* mem_alloc(size_t n) {                  // zeroed, 16-byte header remembers the size
    unsigned char* p = (unsigned char*)plat::raw_alloc(n + 16);
    if (!p) return nullptr;
    *(size_t*)p = n + 16;
    return p + 16;
}
inline void mem_free(void* q) {
    if (!q) return;
    unsigned char* p = (unsigned char*)q - 16;
    plat::raw_free(p, *(size_t*)p);
}
template <class T> inline T* alloc_arr(size_t n) { return (T*)mem_alloc(n * sizeof(T)); }
}
