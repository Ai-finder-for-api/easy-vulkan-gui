// Arena: a bump allocator over a caller-provided buffer. Allocation is a pointer bump; reset() frees everything at once.
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace evgx { namespace mem {

class Arena {
public:
    Arena() = default;
    Arena(void* buffer, size_t size) : base_((uint8_t*)buffer), cap_(size) {}

    // `align` must be a power of two; returns null when the arena is full
    void* alloc(size_t n, size_t align = 16) {
        if (!align || (align & (align - 1))) return nullptr;
        size_t start = (used_ + align - 1) & ~(align - 1);
        if (!base_ || start > cap_ || n > cap_ - start) return nullptr;
        used_ = start + n;
        if (used_ > peak_) peak_ = used_;
        return base_ + start;
    }

    template <class T> T* alloc_n(size_t count) { return (T*)alloc(sizeof(T) * count, alignof(T)); }

    void reset() { used_ = 0; }
    size_t used() const { return used_; }
    size_t capacity() const { return cap_; }
    size_t peak() const { return peak_; }                // largest use since construction (for sizing buffers)
    size_t remaining() const { return used_ > cap_ ? 0 : cap_ - used_; }

    // saves the current position; rewind() returns to it, freeing everything allocated since
    size_t mark() const { return used_; }
    void rewind(size_t m) { if (m <= used_) used_ = m; }

private:
    uint8_t* base_ = nullptr;
    size_t cap_ = 0;
    size_t used_ = 0;
    size_t peak_ = 0;
};

}}  // namespace evgx::mem
