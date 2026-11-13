#include <evg/compat/glm.hpp>
#include <stdio.h>
#include <math.h>   // test only: used as the reference, never by the library
static int fails = 0;
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)
static bool near(float a, float b, float e = 1e-4f) { return fabsf(a - b) <= e; }
int main() {
    float es = 0, ec = 0, et = 0, eq = 0;
    for (float x = -100; x <= 100; x += 0.0137f) {
        float d;
        d = fabsf(glm::sin(x) - sinf(x)); if (d > es) es = d;
        d = fabsf(glm::cos(x) - cosf(x)); if (d > ec) ec = d;
    }
    for (float x = 0.0001f; x < 1e6f; x *= 1.07f) { float d = fabsf(glm::sqrt(x) - sqrtf(x)) / sqrtf(x); if (d > eq) eq = d; }
    for (float x = -1.4f; x <= 1.4f; x += 0.01f) { float d = fabsf(glm::tan(x) - tanf(x)) / (1 + fabsf(tanf(x))); if (d > et) et = d; }
    printf("max err  sin %.2e  cos %.2e  tan(rel) %.2e  sqrt(rel) %.2e\n", es, ec, et, eq);
    CHECK(es < 5e-6f); CHECK(ec < 5e-6f); CHECK(et < 5e-6f); CHECK(eq < 1e-6f);

    glm::vec3 v = glm::vec3(1, 0, 0);
    glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
    glm::vec4 o = r * glm::vec4(v, 1.0f);
    CHECK(near(o.x, 0) && near(o.y, 1) && near(o.z, 0) && near(o.w, 1));

    glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3));
    glm::vec4 p = t * glm::vec4(0, 0, 0, 1);
    CHECK(near(p.x, 1) && near(p.y, 2) && near(p.z, 3));
    glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 4));
    glm::vec4 q = (t * s) * glm::vec4(1, 1, 1, 1);
    CHECK(near(q.x, 3) && near(q.y, 5) && near(q.z, 7));

    glm::vec3 c = glm::cross(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
    CHECK(c == glm::vec3(0, 0, 1));
    CHECK(near(glm::length(glm::vec3(3, 4, 12)), 13.0f));
    CHECK(near(glm::length(glm::normalize(glm::vec3(5, -2, 9))), 1.0f));
    CHECK(near(glm::dot(glm::vec3(1, 2, 3), glm::vec3(4, 5, 6)), 32.0f));

    // perspective: a point on the near plane center maps to z=-1 (GL) / 0 (VK)
    glm::mat4 P = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::vec4 a = P * glm::vec4(0, 0, -0.1f, 1); CHECK(near(a.z / a.w, -1.0f));
    glm::vec4 b = P * glm::vec4(0, 0, -100.0f, 1); CHECK(near(b.z / b.w, 1.0f, 1e-3f));
    glm::mat4 V = glm::perspective_vk(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
    glm::vec4 a2 = V * glm::vec4(0, 0, -0.1f, 1); CHECK(near(a2.z / a2.w, 0.0f));
    glm::vec4 b2 = V * glm::vec4(0, 0, -100.0f, 1); CHECK(near(b2.z / b2.w, 1.0f, 1e-3f));
    glm::vec4 up = V * glm::vec4(0, 1, -1, 1); CHECK(up.y < 0); // Vulkan Y-down

    // lookAt: eye at (0,0,5) looking at origin => origin lands at (0,0,-5)
    glm::mat4 L = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::vec4 lo = L * glm::vec4(0, 0, 0, 1); CHECK(near(lo.z, -5) && near(lo.x, 0) && near(lo.y, 0));

    glm::mat4 O = glm::ortho(0.f, 800.f, 0.f, 600.f, -1.f, 1.f);
    glm::vec4 oc = O * glm::vec4(400, 300, 0, 1); CHECK(near(oc.x, 0) && near(oc.y, 0));
    CHECK(glm::transpose(glm::transpose(L))[3][2] == L[3][2]);


    // inverse
    {
        glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3)) * glm::rotate(glm::mat4(1.0f), 0.7f, glm::vec3(1, 1, 0))
                    * glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 0.5f));
        glm::mat4 I = M * glm::inverse(M);
        for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) CHECK(near(I[c][r], c == r ? 1.f : 0.f, 1e-4f));
        glm::mat4 P2 = glm::perspective(1.0f, 1.5f, 0.1f, 50.f);
        glm::mat4 J = P2 * glm::inverse(P2);
        for (int c = 0; c < 4; c++) for (int r = 0; r < 4; r++) CHECK(near(J[c][r], c == r ? 1.f : 0.f, 1e-3f));
        glm::mat4 S(0.f); glm::mat4 Si = glm::inverse(S); CHECK(Si[0][0] == 1.f); // singular -> identity
    }
    // quaternions
    {
        glm::quat q = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1));
        glm::vec3 v = q * glm::vec3(1, 0, 0);
        CHECK(near(v.x, 0) && near(v.y, 1) && near(v.z, 0));
        glm::mat4 Mq = glm::mat4_cast(q);
        glm::vec4 w4 = Mq * glm::vec4(1, 0, 0, 1);
        CHECK(near(w4.x, 0) && near(w4.y, 1));
        glm::quat q2 = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
        glm::vec3 composed = (q * q2) * glm::vec3(0, 1, 0);          // x-rot first, then z-rot
        glm::vec3 stepwise = q * (q2 * glm::vec3(0, 1, 0));
        CHECK(near(composed.x, stepwise.x) && near(composed.y, stepwise.y) && near(composed.z, stepwise.z));
        glm::quat h = glm::slerp(glm::quat(), q, 0.5f);               // 45 degrees about z
        glm::vec3 hv = h * glm::vec3(1, 0, 0);
        CHECK(near(hv.x, 0.70710678f, 1e-3f) && near(hv.y, 0.70710678f, 1e-3f));
        float ea = 0; for (float a = -6; a <= 6; a += 0.01f) { float d = fabsf(glm::atan2(sinf(a), cosf(a)) - atan2f(sinf(a), cosf(a))); if (d > ea) ea = d; }
        printf("max err atan2 %.2e\n", ea); CHECK(ea < 2e-5f);
    }

    // ---- mat2 / mat3 ----
    {
        glm::mat3 m(glm::vec3(2, 0, 0), glm::vec3(0, 3, 0), glm::vec3(0, 0, 4));
        CHECK(near(glm::determinant(m), 24.f));
        glm::mat3 I = m * glm::inverse(m);
        for (int c = 0; c < 3; c++) for (int r = 0; r < 3; r++) CHECK(near(I[c][r], c == r ? 1.f : 0.f, 1e-5f));
        glm::mat3 R = glm::mat3(glm::mat4_cast(glm::angleAxis(0.6f, glm::vec3(1, 2, 2))));
        glm::mat3 RI = R * glm::transpose(R);                  // rotation: R R^T = I
        for (int c = 0; c < 3; c++) for (int r = 0; r < 3; r++) CHECK(near(RI[c][r], c == r ? 1.f : 0.f, 1e-5f));
        glm::vec3 v = R * glm::vec3(1, 0, 0);
        CHECK(near(glm::length(v), 1.f, 1e-5f));
        glm::mat2 a(glm::vec2(1, 2), glm::vec2(3, 4));
        CHECK(near(glm::determinant(a), -2.f));
        glm::mat2 A = glm::inverse(a) * a;
        CHECK(near(A[0][0], 1.f) && near(A[1][1], 1.f) && near(A[0][1], 0.f) && near(A[1][0], 0.f));
        CHECK(glm::inverse(glm::mat3(0.f))[0][0] == 0.f);     // singular -> zero matrix
        CHECK(near(glm::value_ptr(a)[3], 4.f));
    }
    // ---- GLSL helpers vs libm ----
    {
        float ee = 0, el = 0, ep = 0;
        for (float x = -20; x <= 20; x += 0.013f) { float d = fabsf(glm::exp(x) - expf(x)) / expf(x); if (d > ee) ee = d; }
        for (float x = 1e-3f; x < 1e5f; x *= 1.03f) { float d = fabsf(glm::log(x) - logf(x)); if (d > el) el = d; }
        for (float b = 0.1f; b < 10; b += 0.11f) for (float e = -3; e <= 3; e += 0.37f) { float d = fabsf(glm::pow(b, e) - powf(b, e)) / powf(b, e); if (d > ep) ep = d; }
        printf("max rel err exp %.2e  log abs %.2e  pow %.2e\n", ee, el, ep);
        CHECK(ee < 5e-6f); CHECK(el < 1e-5f); CHECK(ep < 2e-5f);
        CHECK(near(glm::pow(-2.f, 3.f), -8.f) && near(glm::pow(-2.f, 2.f), 4.f));
        CHECK(near(glm::fract(2.75f), 0.75f) && near(glm::fract(-0.25f), 0.75f));
        CHECK(near(glm::mod(-1.f, 3.f), 2.f) && near(glm::sign(-4.f), -1.f) && near(glm::sign(0.f), 0.f));
        CHECK(glm::step(0.5f, 0.4f) == 0.f && glm::step(0.5f, 0.6f) == 1.f);
        CHECK(near(glm::smoothstep(0.f, 1.f, 0.5f), 0.5f) && glm::smoothstep(0.f, 1.f, -3.f) == 0.f);
        glm::vec3 r = glm::reflect(glm::vec3(1, -1, 0), glm::vec3(0, 1, 0));
        CHECK(near(r.x, 1) && near(r.y, 1) && near(r.z, 0));
        glm::vec3 t = glm::refract(glm::normalize(glm::vec3(1, -1, 0)), glm::vec3(0, 1, 0), 1.0f);   // eta=1: straight through
        CHECK(near(t.x, 0.70710678f, 1e-4f) && near(t.y, -0.70710678f, 1e-4f));
        glm::vec3 c = glm::clamp(glm::vec3(-1, 0.5f, 3), 0.f, 1.f);
        CHECK(c.x == 0.f && c.y == 0.5f && c.z == 1.f);
        glm::vec3 mn = glm::min(glm::vec3(1, 5, 3), glm::vec3(2, 4, 3));
        CHECK(mn.x == 1.f && mn.y == 4.f && mn.z == 3.f);
        glm::vec3 mx = glm::max(glm::vec3(1, 5, 3), 2.f);
        CHECK(mx.x == 2.f && mx.y == 5.f && mx.z == 3.f);
        glm::vec4 ab = glm::abs(glm::vec4(-1, 2, -3, 4));
        CHECK(ab.x == 1 && ab.z == 3);
        glm::vec2 fl = glm::floor(glm::vec2(1.7f, -1.2f));
        CHECK(fl.x == 1 && fl.y == -2);
        CHECK(glm::ceil(1.2f) == 2.f);
        CHECK(near(glm::mix(glm::vec3(0, 0, 0), glm::vec3(2, 4, 6), 0.5f).z, 3));
    }
    // ---- ortho with default near/far ----
    {
        glm::mat4 o = glm::ortho(0.f, 4.f, 0.f, 2.f);
        glm::vec4 p = o * glm::vec4(4, 2, -0.5f, 1);
        CHECK(near(p.x, 1) && near(p.y, 1) && near(p.z, 0.5f));   // clip z = -2/(f-n)*z - (f+n)/(f-n) = 0.5
    }
    printf(fails ? "math: %d FAILED\n" : "math: all passed\n", fails);
    return fails != 0;
}
