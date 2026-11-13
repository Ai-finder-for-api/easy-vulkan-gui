// String helpers without the STL: a non-owning view, trimming, splitting, prefix/suffix tests, number parsing and formatting.
#pragma once
#include <stdint.h>
#include <stddef.h>

namespace evgx { namespace str {

struct View {
    const char* data = nullptr;
    size_t size = 0;
    constexpr View() = default;
    constexpr View(const char* d, size_t n) : data(d), size(n) {}
    explicit View(const char* z) : data(z), size(0) { if (z) while (z[size]) size++; }
    char operator[](size_t i) const { return data[i]; }
    bool empty() const { return size == 0; }
    View sub(size_t pos, size_t n = (size_t)-1) const {
        if (pos > size) pos = size;
        if (n > size - pos) n = size - pos;
        return View(data + pos, n);
    }
};

inline bool equal(View a, View b) {
    if (a.size != b.size) return false;
    for (size_t i = 0; i < a.size; i++) if (a.data[i] != b.data[i]) return false;
    return true;
}

inline bool equal(View a, const char* z) { return equal(a, View(z)); }

inline bool starts_with(View s, View prefix) {
    return prefix.size <= s.size && equal(s.sub(0, prefix.size), prefix);
}

inline bool ends_with(View s, View suffix) {
    return suffix.size <= s.size && equal(s.sub(s.size - suffix.size), suffix);
}

// index of the first occurrence of `c`, or (size_t)-1
inline size_t find(View s, char c, size_t from = 0) {
    for (size_t i = from; i < s.size; i++) if (s.data[i] == c) return i;
    return (size_t)-1;
}

// removes spaces, tabs, and line endings from both ends
inline View trim(View s) {
    size_t a = 0, b = s.size;
    auto ws = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
    while (a < b && ws(s.data[a])) a++;
    while (b > a && ws(s.data[b - 1])) b--;
    return View(s.data + a, b - a);
}

// calls fn(piece) for each field between `sep` characters (empty fields included)
template <class F> void split(View s, char sep, F fn) {
    size_t start = 0;
    for (size_t i = 0; i <= s.size; i++) {
        if (i == s.size || s.data[i] == sep) {
            fn(View(s.data + start, i - start));
            start = i + 1;
        }
    }
}

// parses an optionally signed decimal integer; false on empty input, junk, or overflow of 64 bits
inline bool to_i64(View s, int64_t& out) {
    s = trim(s);
    if (s.empty()) return false;
    size_t i = 0;
    bool neg = false;
    if (s.data[0] == '-' || s.data[0] == '+') { neg = s.data[0] == '-'; i = 1; }
    if (i == s.size) return false;
    uint64_t v = 0;
    for (; i < s.size; i++) {
        char c = s.data[i];
        if (c < '0' || c > '9') return false;
        uint64_t d = (uint64_t)(c - '0');
        if (v > (UINT64_MAX - d) / 10) return false;
        v = v * 10 + d;
    }
    if (!neg && v > (uint64_t)INT64_MAX) return false;
    if (neg && v > (uint64_t)INT64_MAX + 1) return false;
    out = neg ? (int64_t)(0 - v) : (int64_t)v;
    return true;
}

// parses a decimal floating-point number with optional sign, fraction and exponent (no inf/nan)
inline bool to_f64(View s, double& out) {
    s = trim(s);
    size_t i = 0;
    bool neg = false;
    if (i < s.size && (s.data[i] == '-' || s.data[i] == '+')) { neg = s.data[i] == '-'; i++; }
    double v = 0;
    bool digits = false;
    while (i < s.size && s.data[i] >= '0' && s.data[i] <= '9') { v = v * 10 + (s.data[i] - '0'); i++; digits = true; }
    if (i < s.size && s.data[i] == '.') {
        i++;
        double f = 0.1;
        while (i < s.size && s.data[i] >= '0' && s.data[i] <= '9') { v += (s.data[i] - '0') * f; f *= 0.1; i++; digits = true; }
    }
    if (!digits) return false;
    if (i < s.size && (s.data[i] == 'e' || s.data[i] == 'E')) {
        i++;
        bool eneg = false;
        if (i < s.size && (s.data[i] == '-' || s.data[i] == '+')) { eneg = s.data[i] == '-'; i++; }
        int ex = 0;
        bool edigits = false;
        while (i < s.size && s.data[i] >= '0' && s.data[i] <= '9') {
            if (ex < 10000) ex = ex * 10 + (s.data[i] - '0');
            i++;
            edigits = true;
        }
        if (!edigits) return false;
        double m = 1;
        for (int k = 0; k < ex; k++) m *= 10;
        v = eneg ? v / m : v * m;
    }
    if (i != s.size) return false;
    out = neg ? -v : v;
    return true;
}

// writes the decimal form of v into buf (NUL-terminated); returns the length without the NUL, or 0 if it does not fit
inline size_t format_i64(int64_t v, char* buf, size_t cap) {
    char tmp[24];
    size_t n = 0;
    uint64_t u = v < 0 ? (uint64_t)0 - (uint64_t)v : (uint64_t)v;
    do { tmp[n++] = (char)('0' + u % 10); u /= 10; } while (u && n < sizeof tmp);
    if (v < 0) tmp[n++] = '-';
    if (n + 1 > cap) return 0;
    for (size_t i = 0; i < n; i++) buf[i] = tmp[n - 1 - i];
    buf[n] = 0;
    return n;
}

}}  // namespace evgx::str
