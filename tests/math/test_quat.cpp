// Quaternions: rotation of vectors, composition order, slerp, matrix conversion, normalization.
#include "../common/check.hpp"
#include <evg/compat/glm.hpp>

int main() {
    using evgtest::near;
    glm::quat q = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1));
    glm::vec3 v = q * glm::vec3(1, 0, 0);
    CHECK(near(v.x, 0) && near(v.y, 1) && near(v.z, 0));

    // the matrix form agrees with the quaternion form
    glm::vec4 w = glm::mat4_cast(q) * glm::vec4(1, 0, 0, 1);
    CHECK(near(w.x, 0) && near(w.y, 1));

    // (a * b) applied to a vector = b first, then a
    glm::quat q2 = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
    glm::vec3 composed = (q * q2) * glm::vec3(0, 1, 0);
    glm::vec3 stepwise = q * (q2 * glm::vec3(0, 1, 0));
    CHECK(near(composed.x, stepwise.x) && near(composed.y, stepwise.y) && near(composed.z, stepwise.z));

    // slerp halfway between identity and a 90-degree turn is a 45-degree turn
    glm::quat h = glm::slerp(glm::quat(), q, 0.5f);
    glm::vec3 hv = h * glm::vec3(1, 0, 0);
    CHECK(near(hv.x, 0.70710678f, 1e-3f) && near(hv.y, 0.70710678f, 1e-3f));

    // normalize gives unit length; zero stays the identity
    glm::quat n = glm::normalize(glm::quat(2, 0, 0, 0));
    CHECK(near(n.w, 1.f));
    glm::quat z = glm::normalize(glm::quat(0, 0, 0, 0));
    CHECK(near(z.w, 1.f));

    // conjugate undoes a rotation
    glm::vec3 back = glm::conjugate(q) * (q * glm::vec3(0.3f, -0.4f, 0.5f));
    CHECK(near(back.x, 0.3f) && near(back.y, -0.4f) && near(back.z, 0.5f));

    return evgtest::finish("math/quat");
}
