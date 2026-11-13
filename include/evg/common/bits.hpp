// Bit tricks: population count, leading/trailing zeros, power-of-two tests, alignment, rotations. Portable, no intrinsics.
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace evgx { namespace bits {

inline int popcount32(uint32_t x) {
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    return (int)((((x + (x >> 4)) & 0x0F0F0F0Fu) * 0x01010101u) >> 24);
}

inline int popcount64(uint64_t x) { return popcount32((uint32_t)x) + popcount32((uint32_t)(x >> 32)); }

// number of leading zero bits; 32 for x == 0
inline int clz32(uint32_t x) {
    if (!x) return 32;
    int n = 0;
    while (!(x & 0x80000000u)) { x <<= 1; n++; }
    return n;
}

// number of trailing zero bits; 32 for x == 0
inline int ctz32(uint32_t x) {
    if (!x) return 32;
    int n = 0;
    while (!(x & 1u)) { x >>= 1; n++; }
    return n;
}

inline bool is_pow2(uint64_t x) { return x != 0 && (x & (x - 1)) == 0; }

// smallest power of two >= x (0 maps to 1); 0 when the result would overflow 64 bits
inline uint64_t next_pow2(uint64_t x) {
    if (x <= 1) return 1;
    x--;
    x |= x >> 1; x |= x >> 2; x |= x >> 4; x |= x >> 8; x |= x >> 16; x |= x >> 32;
    x++;
    return x == 0 ? 0 : x;
}

// rounds n up to a multiple of align (align must be a power of two)
inline size_t align_up(size_t n, size_t align) { return (n + align - 1) & ~(align - 1); }

inline uint32_t rotl32(uint32_t x, int r) { r &= 31; return r ? (x << r) | (x >> (32 - r)) : x; }
inline uint32_t rotr32(uint32_t x, int r) { r &= 31; return r ? (x >> r) | (x << (32 - r)) : x; }
inline uint64_t rotl64(uint64_t x, int r) { r &= 63; return r ? (x << r) | (x >> (64 - r)) : x; }

// reverses the byte order of a 32-bit value
inline uint32_t bswap32(uint32_t x) {
    return ((x & 0x000000FFu) << 24) | ((x & 0x0000FF00u) << 8) | ((x & 0x00FF0000u) >> 8) | ((x & 0xFF000000u) >> 24);
}

}}  // namespace evgx::bits
