// GLSL-style helpers and vector overloads: exp/log/pow accuracy, fract/mod/step/smoothstep, component-wise functions, reflect/refract.
#include <math.h>
#include "../common/check.hpp"
#include <evg/compat/glm.hpp>

int main() {
    using evgtest::near;
    float ee = 0, el = 0, ep = 0;
    for (float x = -20; x <= 20; x += 0.013f) ee = fmaxf(ee, fabsf(glm::exp(x) - expf(x)) / expf(x));
    for (float x = 1e-3f; x < 1e5f; x *= 1.03f) el = fmaxf(el, fabsf(glm::log(x) - logf(x)));
    for (float b = 0.1f; b < 10; b += 0.11f)
        for (float e = -3; e <= 3; e += 0.37f) ep = fmaxf(ep, fabsf(glm::pow(b, e) - powf(b, e)) / powf(b, e));
    CHECK(ee < 5e-6f);
    CHECK(el < 1e-5f);
    CHECK(ep < 2e-5f);
    CHECK(near(glm::pow(-2.f, 3.f), -8.f) && near(glm::pow(-2.f, 2.f), 4.f));

    CHECK(near(glm::fract(2.75f), 0.75f) && near(glm::fract(-0.25f), 0.75f));
    CHECK(near(glm::mod(-1.f, 3.f), 2.f));
    CHECK(near(glm::sign(-4.f), -1.f) && near(glm::sign(0.f), 0.f));
    CHECK(glm::step(0.5f, 0.4f) == 0.f && glm::step(0.5f, 0.6f) == 1.f);
    CHECK(near(glm::smoothstep(0.f, 1.f, 0.5f), 0.5f) && glm::smoothstep(0.f, 1.f, -3.f) == 0.f);

    // component-wise
    glm::vec3 c = glm::clamp(glm::vec3(-1, 0.5f, 3), 0.f, 1.f);
    CHECK(c.x == 0.f && c.y == 0.5f && c.z == 1.f);
    glm::vec3 mn = glm::min(glm::vec3(1, 5, 3), glm::vec3(2, 4, 3));
    CHECK(mn.x == 1.f && mn.y == 4.f && mn.z == 3.f);
    glm::vec3 mx = glm::max(glm::vec3(1, 5, 3), 2.f);
    CHECK(mx.x == 2.f && mx.y == 5.f && mx.z == 3.f);
    glm::vec4 ab = glm::abs(glm::vec4(-1, 2, -3, 4));
    CHECK(ab.x == 1.f && ab.z == 3.f);
    glm::vec2 fl = glm::floor(glm::vec2(1.7f, -1.2f));
    CHECK(fl.x == 1.f && fl.y == -2.f);
    CHECK(glm::ceil(1.2f) == 2.f);
    glm::vec3 m = glm::mix(glm::vec3(0, 0, 0), glm::vec3(2, 4, 6), 0.5f);
    CHECK(near(m.z, 3.f));

    // reflection and refraction
    glm::vec3 r = glm::reflect(glm::vec3(1, -1, 0), glm::vec3(0, 1, 0));
    CHECK(near(r.x, 1.f) && near(r.y, 1.f) && near(r.z, 0.f));
    glm::vec3 t = glm::refract(glm::normalize(glm::vec3(1, -1, 0)), glm::vec3(0, 1, 0), 1.0f);   // eta 1: straight through
    CHECK(near(t.x, 0.70710678f, 1e-4f) && near(t.y, -0.70710678f, 1e-4f));
    glm::vec3 total = glm::refract(glm::normalize(glm::vec3(1, -0.1f, 0)), glm::vec3(0, 1, 0), 10.f);  // total internal reflection case
    CHECK(total.x == 0.f && total.y == 0.f && total.z == 0.f);

    return evgtest::finish("math/glsl");
}
