// Fixed-capacity ring buffer: push at the back, pop from the front, no allocation after construction.
// Two overflow policies: push() refuses when full; push_overwrite() drops the oldest element.
#pragma once
#include <stddef.h>

namespace evgx { namespace ds {

template <class T, size_t N>
class Ring {
    static_assert(N > 0, "Ring needs a capacity");
public:
    bool push(const T& v) {
        if (count_ == N) return false;
        data_[(head_ + count_) % N] = v;
        count_++;
        return true;
    }

    // always succeeds: when full, the oldest element is discarded
    void push_overwrite(const T& v) {
        if (count_ == N) { head_ = (head_ + 1) % N; count_--; }
        push(v);
    }

    bool pop(T& out) {
        if (!count_) return false;
        out = data_[head_];
        head_ = (head_ + 1) % N;
        count_--;
        return true;
    }

    // element i counted from the front (0 = oldest); i must be < size()
    const T& at(size_t i) const { return data_[(head_ + i) % N]; }

    size_t size() const { return count_; }
    size_t capacity() const { return N; }
    bool empty() const { return count_ == 0; }
    bool full() const { return count_ == N; }
    void clear() { head_ = 0; count_ = 0; }

private:
    T data_[N];
    size_t head_ = 0;
    size_t count_ = 0;
};

}}  // namespace evgx::ds
