// Bits, hashing, random numbers, arena allocator and ring buffer.
#include "../common/check.hpp"
#include <evg/common/bits.hpp>
#include <evg/common/hash.hpp>
#include <evg/common/random.hpp>
#include <evg/common/arena.hpp>
#include <evg/common/ring.hpp>

using namespace evgx;

int main() {
    // bits
    CHECK(bits::popcount32(0) == 0 && bits::popcount32(0xFFFFFFFFu) == 32 && bits::popcount32(0x80000001u) == 2);
    CHECK(bits::popcount64(0xF0F0F0F0F0F0F0F0ull) == 32);
    CHECK(bits::clz32(1) == 31 && bits::clz32(0x80000000u) == 0 && bits::clz32(0) == 32);
    CHECK(bits::ctz32(8) == 3 && bits::ctz32(0) == 32);
    CHECK(bits::is_pow2(1) && bits::is_pow2(1024) && !bits::is_pow2(0) && !bits::is_pow2(12));
    CHECK(bits::next_pow2(0) == 1 && bits::next_pow2(1) == 1 && bits::next_pow2(5) == 8 && bits::next_pow2(1024) == 1024);
    CHECK(bits::align_up(13, 8) == 16 && bits::align_up(16, 8) == 16);
    CHECK(bits::rotl32(0x80000001u, 1) == 0x00000003u && bits::rotr32(3, 1) == 0x80000001u);
    CHECK(bits::bswap32(0x11223344u) == 0x44332211u);

    // hashing: published test vectors
    CHECK(hash::fnv1a32("", 0) == 2166136261u);
    CHECK(hash::fnv1a32("a", 1) == 0xE40C292Cu);
    CHECK(hash::fnv1a64("a", 1) == 0xAF63DC4C8601EC8Cull);
    CHECK(hash::crc32("123456789", 9) == 0xCBF43926u);     // the standard check value
    CHECK(hash::crc32("", 0) == 0);
    CHECK(hash::crc32("6789", 4, hash::crc32("12345", 5)) == 0xCBF43926u);   // seeding continues a stream
    CHECK(hash::combine(1, 2) != hash::combine(2, 1));

    // random: same seed, same sequence; values in range; uniform in [0,1)
    rnd::Rng a(42), b(42), c(43);
    CHECK(a.next() == b.next() && a.next() == b.next());
    CHECK(c.next() != rnd::Rng(42).next());
    bool inRange = true, unit = true;
    rnd::Rng r(7);
    for (int i = 0; i < 1000; i++) {
        int v = r.range(-3, 3);
        if (v < -3 || v > 3) inRange = false;
        float u = r.uniform();
        if (u < 0.f || u >= 1.f) unit = false;
    }
    CHECK(inRange && unit);
    CHECK(rnd::Rng(5).range(4, 4) == 4);
    CHECK(rnd::Rng(5).range(9, 2) >= 2 && rnd::Rng(5).range(9, 2) <= 9);   // reversed bounds are accepted

    // arena: bump allocation with alignment, capacity, peak, mark and rewind
    alignas(64) uint8_t storage[256];
    mem::Arena arena(storage, sizeof storage);
    void* p1 = arena.alloc(10, 16);
    void* p2 = arena.alloc(4, 64);
    CHECK(p1 != nullptr && p2 != nullptr);
    CHECK(((uintptr_t)p2 % 64) == 0);
    CHECK(arena.used() >= 14);
    size_t mark = arena.mark();
    CHECK(arena.alloc(150, 1) != nullptr);                  // 68 + 150 fits in 256
    CHECK(arena.alloc(1000, 1) == nullptr);                 // does not fit
    CHECK(arena.peak() >= 218);
    arena.rewind(mark);
    CHECK(arena.used() == mark);
    arena.reset();
    CHECK(arena.used() == 0 && arena.remaining() == sizeof storage);
    CHECK(arena.alloc(8, 3) == nullptr);                    // non-power-of-two alignment is refused
    int* nums = arena.alloc_n<int>(4);
    CHECK(nums != nullptr && ((uintptr_t)nums % alignof(int)) == 0);

    // ring buffer: fifo order, refusal when full, overwrite of the oldest
    ds::Ring<int, 3> q;
    CHECK(q.push(1) && q.push(2) && q.push(3));
    CHECK(q.full() && !q.push(4));
    int v = 0;
    CHECK(q.pop(v) && v == 1);
    q.push_overwrite(5);                                    // drops nothing: one slot is free after the pop
    CHECK(q.size() == 3 && q.at(0) == 2 && q.at(2) == 5);
    q.push_overwrite(6);                                    // full again: drops the oldest (2)
    CHECK(q.at(0) == 3 && q.at(1) == 5 && q.at(2) == 6);
    CHECK(q.pop(v) && v == 3 && q.pop(v) && v == 5 && q.pop(v) && v == 6);
    CHECK(q.empty() && !q.pop(v));
    CHECK(q.capacity() == 3);

    return evgtest::finish("common/bits_hash_arena");
}
