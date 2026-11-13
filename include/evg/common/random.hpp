// Random numbers: SplitMix64 (seeding) and xoshiro256** (generation). Deterministic for a given seed; not cryptographic.
#pragma once
#include <stdint.h>

namespace evgx { namespace rnd {

inline uint64_t splitmix64(uint64_t& state) {
    uint64_t z = (state += 0x9e3779b97f4a7c15ull);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
    return z ^ (z >> 31);
}

class Rng {
public:
    explicit Rng(uint64_t seed = 1) { seed_(seed); }

    void seed(uint64_t s) { seed_(s); }

    uint64_t next() {
        const uint64_t result = rotl(s_[1] * 5, 7) * 9;
        const uint64_t t = s_[1] << 17;
        s_[2] ^= s_[0];
        s_[3] ^= s_[1];
        s_[1] ^= s_[2];
        s_[0] ^= s_[3];
        s_[2] ^= t;
        s_[3] = rotl(s_[3], 45);
        return result;
    }

    // uniform float in [0, 1)
    float uniform() { return (float)(next() >> 40) * (1.0f / 16777216.0f); }

    // uniform integer in [lo, hi], inclusive; lo > hi swaps them
    int range(int lo, int hi) {
        if (lo > hi) { int t = lo; lo = hi; hi = t; }
        uint64_t span = (uint64_t)((int64_t)hi - (int64_t)lo) + 1;
        return (int)((int64_t)lo + (int64_t)(next() % span));
    }

private:
    static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
    void seed_(uint64_t s) {
        for (int i = 0; i < 4; i++) s_[i] = splitmix64(s);
        if (!(s_[0] | s_[1] | s_[2] | s_[3])) s_[0] = 1;   // the all-zero state is not allowed
    }
    uint64_t s_[4];
};

}}  // namespace evgx::rnd
