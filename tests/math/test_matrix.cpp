// 4x4, 3x3 and 2x2 matrices: transforms, inverse, projections, lookAt, and the value layout.
#include "../common/check.hpp"
#include <evg/compat/glm.hpp>

int main() {
    using evgtest::near;

    // translate * scale applied to a point: scale first (about the origin), then translate
    glm::mat4 ts = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3)) * glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 4));
    glm::vec4 p = ts * glm::vec4(1, 1, 1, 1);
    CHECK(near(p.x, 3) && near(p.y, 5) && near(p.z, 7));

    // rotation by 90 degrees about z maps +x to +y
    glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
    glm::vec4 q = rz * glm::vec4(1, 0, 0, 1);
    CHECK(near(q.x, 0) && near(q.y, 1) && near(q.z, 0));

    // general inverse: M * inverse(M) = I
    glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3)) * glm::rotate(glm::mat4(1.0f), 0.7f, glm::vec3(1, 1, 0))
                * glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 0.5f));
    glm::mat4 I = M * glm::inverse(M);
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++) CHECK(near(I[c][r], c == r ? 1.f : 0.f, 1e-4f));
    CHECK(glm::inverse(glm::mat4(0.f))[0][0] == 1.f);       // singular matrix falls back to identity

    // OpenGL projection: near plane maps to -1, far to +1; Vulkan projection: near to 0, Y flipped
    glm::mat4 P = glm::perspective(glm::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f);
    glm::vec4 a = P * glm::vec4(0, 0, -0.1f, 1);
    CHECK(near(a.z / a.w, -1.0f));
    glm::vec4 b = P * glm::vec4(0, 0, -100.0f, 1);
    CHECK(near(b.z / b.w, 1.0f, 1e-3f));
    glm::mat4 V = glm::perspective_vk(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
    glm::vec4 a2 = V * glm::vec4(0, 0, -0.1f, 1);
    CHECK(near(a2.z / a2.w, 0.0f));
    glm::vec4 up = V * glm::vec4(0, 1, -1, 1);
    CHECK(up.y < 0.f);

    // lookAt: eye at (0,0,5) looking at the origin puts the origin 5 units in front
    glm::mat4 L = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::vec4 lo = L * glm::vec4(0, 0, 0, 1);
    CHECK(near(lo.x, 0) && near(lo.y, 0) && near(lo.z, -5));

    // orthographic: (4,2) is the far corner of a 4x2 view, mapping to (1,1) in clip space
    glm::mat4 O = glm::ortho(0.f, 4.f, 0.f, 2.f);
    glm::vec4 op = O * glm::vec4(4, 2, -0.5f, 1);
    CHECK(near(op.x, 1) && near(op.y, 1) && near(op.z, 0.5f));

    // transpose twice is the identity; value_ptr exposes column-major data
    CHECK(glm::transpose(glm::transpose(L))[3][2] == L[3][2]);
    CHECK(glm::value_ptr(L)[14] == L[3][2]);

    // 3x3: determinant, inverse, rotation orthonormality
    glm::mat3 m3(glm::vec3(2, 0, 0), glm::vec3(0, 3, 0), glm::vec3(0, 0, 4));
    CHECK(near(glm::determinant(m3), 24.f));
    glm::mat3 i3 = m3 * glm::inverse(m3);
    for (int c = 0; c < 3; c++)
        for (int r = 0; r < 3; r++) CHECK(near(i3[c][r], c == r ? 1.f : 0.f, 1e-5f));
    glm::mat3 R = glm::mat3(glm::mat4_cast(glm::angleAxis(0.6f, glm::vec3(1, 2, 2))));
    glm::mat3 RRt = R * glm::transpose(R);
    for (int c = 0; c < 3; c++)
        for (int r = 0; r < 3; r++) CHECK(near(RRt[c][r], c == r ? 1.f : 0.f, 1e-5f));

    // 2x2
    glm::mat2 m2(glm::vec2(1, 2), glm::vec2(3, 4));
    CHECK(near(glm::determinant(m2), -2.f));
    glm::mat2 i2 = glm::inverse(m2) * m2;
    CHECK(near(i2[0][0], 1.f) && near(i2[1][1], 1.f) && near(i2[0][1], 0.f) && near(i2[1][0], 0.f));

    return evgtest::finish("math/matrix");
}
