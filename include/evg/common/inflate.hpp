// DEFLATE and zlib decompression (RFC 1951 / RFC 1950). Stored, fixed-Huffman and dynamic-Huffman blocks.
// The zlib wrapper is checked (header and Adler-32). No preset dictionary support.
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace evgx { namespace inflate {

// decompresses a zlib stream. On success `out` receives a newly allocated buffer of `outn` bytes; release it
// with free_buffer(). On failure `err` points at a static message and `out` is null.
bool zlib_decompress(const uint8_t* in, size_t n, uint8_t*& out, size_t& outn, const char** err);

// decompresses a raw DEFLATE stream (no zlib header, no checksum)
bool deflate_decompress(const uint8_t* in, size_t n, uint8_t*& out, size_t& outn, const char** err);

void free_buffer(uint8_t* p);

// CRC-32 (IEEE) and Adler-32, the checksums used by PNG and zlib
uint32_t crc32_ieee(const uint8_t* data, size_t n);
uint32_t adler32(const uint8_t* data, size_t n);

}}  // namespace evgx::inflate
