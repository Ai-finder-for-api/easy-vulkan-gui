// Scalar math against the C library (reference only; the library itself never calls libm).
#include <math.h>
#include "../common/check.hpp"
#include <evg/compat/glm.hpp>

int main() {
    using evgtest::near;
    // trig over several periods
    float es = 0, ec = 0, et = 0;
    for (float x = -50; x <= 50; x += 0.01f) {
        es = fmaxf(es, fabsf(glm::sin(x) - sinf(x)));
        ec = fmaxf(ec, fabsf(glm::cos(x) - cosf(x)));
    }
    for (float x = -1.4f; x <= 1.4f; x += 0.01f) et = fmaxf(et, fabsf(glm::tan(x) - tanf(x)) / (1.f + fabsf(tanf(x))));
    CHECK(es < 5e-6f);
    CHECK(ec < 5e-6f);
    CHECK(et < 5e-6f);

    // sqrt across magnitudes
    float eq = 0;
    for (float x = 1e-4f; x < 1e6f; x *= 1.07f) eq = fmaxf(eq, fabsf(glm::sqrt(x) - sqrtf(x)) / sqrtf(x));
    CHECK(eq < 1e-6f);
    CHECK(glm::sqrt(-4.f) == 0.f);                        // negative input clamps to zero

    // inverse trig
    CHECK(near(glm::atan2(1.f, 1.f), 0.7853982f, 2e-5f));
    CHECK(near(glm::atan2(-1.f, -1.f), -2.3561945f, 2e-5f));
    CHECK(near(glm::asin(1.f), 1.5707963f, 2e-5f));
    CHECK(near(glm::acos(0.f), 1.5707963f, 2e-5f));

    // angle conversion and clamping helpers
    CHECK(near(glm::radians(180.f), 3.1415927f, 1e-6f));
    CHECK(near(glm::degrees(glm::pi), 180.f, 1e-3f));
    CHECK(glm::clamp(5.f, 0.f, 1.f) == 1.f);
    CHECK(glm::min(2, 3) == 2 && glm::max(2, 3) == 3);
    CHECK(near(glm::mix(1.f, 3.f, 0.25f), 1.5f));

    return evgtest::finish("math/scalar");
}
