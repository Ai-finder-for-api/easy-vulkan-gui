// Minimal test harness shared by every evg test file: CHECK records failures, finish() prints the summary.
#pragma once
#include <stdio.h>
#include <evg/evg.hpp>

namespace evgtest {
inline int& failures() { static int f = 0; return f; }
inline int& checks() { static int c = 0; return c; }

inline bool near(float a, float b, float eps = 1e-4f) { return (a > b ? a - b : b - a) <= eps; }

inline int finish(const char* name) {
    if (failures()) printf("%s: %d of %d checks FAILED\n", name, failures(), checks());
    else printf("%s: all %d checks passed\n", name, checks());
    return failures() ? 1 : 0;
}
}  // namespace evgtest

#define CHECK(c) do { evgtest::checks()++; if (!(c)) { evgtest::failures()++; \
    printf("FAIL %s:%d: %s   [evg error: '%s']\n", __FILE__, __LINE__, #c, evg.error()); } } while (0)
