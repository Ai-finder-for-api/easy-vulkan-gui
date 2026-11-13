// UTF-8: decode and encode code points, count code points, validate sequences.
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace evgx { namespace utf8 {

// decodes one code point at s[i] (n bytes available). Returns the code point and advances i; invalid bytes
// yield U+FFFD and advance by one byte.
inline uint32_t decode(const uint8_t* s, size_t n, size_t& i) {
    uint8_t c = s[i];
    if (c < 0x80) { i += 1; return c; }
    int len = 0;
    uint32_t cp = 0, minv = 0;
    if ((c & 0xE0) == 0xC0) { len = 2; cp = c & 0x1F; minv = 0x80; }
    else if ((c & 0xF0) == 0xE0) { len = 3; cp = c & 0x0F; minv = 0x800; }
    else if ((c & 0xF8) == 0xF0) { len = 4; cp = c & 0x07; minv = 0x10000; }
    else { i += 1; return 0xFFFD; }
    if (i + (size_t)len > n) { i += 1; return 0xFFFD; }
    for (int k = 1; k < len; k++) {
        uint8_t cc = s[i + k];
        if ((cc & 0xC0) != 0x80) { i += 1; return 0xFFFD; }
        cp = (cp << 6) | (cc & 0x3F);
    }
    if (cp < minv || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) { i += 1; return 0xFFFD; }   // overlong, out of range, surrogate
    i += (size_t)len;
    return cp;
}

// encodes cp into out (at least 4 bytes); returns the number of bytes written (0 for invalid code points)
inline int encode(uint32_t cp, uint8_t* out) {
    if (cp < 0x80) { out[0] = (uint8_t)cp; return 1; }
    if (cp < 0x800) { out[0] = (uint8_t)(0xC0 | (cp >> 6)); out[1] = (uint8_t)(0x80 | (cp & 0x3F)); return 2; }
    if (cp >= 0xD800 && cp <= 0xDFFF) return 0;
    if (cp < 0x10000) {
        out[0] = (uint8_t)(0xE0 | (cp >> 12));
        out[1] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (uint8_t)(0x80 | (cp & 0x3F));
        return 3;
    }
    if (cp <= 0x10FFFF) {
        out[0] = (uint8_t)(0xF0 | (cp >> 18));
        out[1] = (uint8_t)(0x80 | ((cp >> 12) & 0x3F));
        out[2] = (uint8_t)(0x80 | ((cp >> 6) & 0x3F));
        out[3] = (uint8_t)(0x80 | (cp & 0x3F));
        return 4;
    }
    return 0;
}

// number of code points in a byte string (invalid bytes count as one each)
inline size_t count(const uint8_t* s, size_t n) {
    size_t i = 0, c = 0;
    while (i < n) { decode(s, n, i); c++; }
    return c;
}

// true when the whole byte string is well-formed UTF-8. A rejected byte always decodes to U+FFFD and advances
// one byte; a valid U+FFFD advances three, so the advance tells the two apart.
inline bool valid(const uint8_t* s, size_t n) {
    size_t i = 0;
    while (i < n) {
        size_t before = i;
        uint32_t cp = decode(s, n, i);
        if (cp == 0xFFFD && i - before == 1) return false;
    }
    return true;
}

}}  // namespace evgx::utf8
