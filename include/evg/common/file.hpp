// File helpers: read a whole file into memory evg allocates, write a buffer, and query a size. Implemented in src/common/file.cpp.
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace evgx { namespace file {

// returns the size in bytes, or -1 when the file cannot be opened
long long size(const char* path);

// reads the whole file into a newly allocated buffer (free with mem_free); false on failure
bool read_all(const char* path, uint8_t*& data, size_t& n);

// writes the buffer, replacing the file; false on failure
bool write_all(const char* path, const void* data, size_t n);

// releases a buffer obtained from read_all
void free_buffer(uint8_t* data, size_t n);

}}  // namespace evgx::file
