// GLM-compatible surface: `glm::vec3`, `glm::translate`, ... backed by evg's own math.
#pragma once
#include "../math.hpp"
namespace glm {
using namespace evgx::math;
using vec2 = evgx::math::vec2<float>;
using vec3 = evgx::math::vec3<float>;
using vec4 = evgx::math::vec4<float>;
}
