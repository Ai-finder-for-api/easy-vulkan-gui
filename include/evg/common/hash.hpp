// Hashing: FNV-1a (32 and 64 bit), CRC-32 (IEEE, table built on first use), and a hash combiner for composite keys.
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace evgx { namespace hash {

inline uint32_t fnv1a32(const void* data, size_t n) {
    const uint8_t* p = (const uint8_t*)data;
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < n; i++) { h ^= p[i]; h *= 16777619u; }
    return h;
}

inline uint64_t fnv1a64(const void* data, size_t n) {
    const uint8_t* p = (const uint8_t*)data;
    uint64_t h = 14695981039346656037ull;
    for (size_t i = 0; i < n; i++) { h ^= p[i]; h *= 1099511628211ull; }
    return h;
}

// CRC-32 with the IEEE 802.3 polynomial (same results as zlib's crc32)
inline uint32_t crc32(const void* data, size_t n, uint32_t seed = 0) {
    static uint32_t table[256];
    static bool ready = false;
    if (!ready) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int k = 0; k < 8; k++) c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1;
            table[i] = c;
        }
        ready = true;
    }
    const uint8_t* p = (const uint8_t*)data;
    uint32_t c = ~seed;
    for (size_t i = 0; i < n; i++) c = table[(c ^ p[i]) & 0xFF] ^ (c >> 8);
    return ~c;
}

// mixes a new value into a running hash (boost-style combiner, 64 bit)
inline uint64_t combine(uint64_t seed, uint64_t value) {
    return seed ^ (value + 0x9e3779b97f4a7c15ull + (seed << 6) + (seed >> 2));
}

}}  // namespace evgx::hash
