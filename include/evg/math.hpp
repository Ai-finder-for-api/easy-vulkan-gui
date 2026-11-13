// evg math (GLM-compatible). Umbrella header: includes the parts in dependency order.
//   math/scalar.hpp  constants, sqrt/sin/cos/atan, min/max/clamp/mix
//   math/vec.hpp     vec2/vec3/vec4, operators, dot/cross/length/normalize
//   math/glsl.hpp    exp/log/pow, fract/mod/step/smoothstep, component-wise vector overloads, reflect/refract
//   math/quat.hpp    quaternions
//   math/mat4.hpp    4x4 matrices, transforms, projections, lookAt, inverse
//   math/mat23.hpp   2x2 and 3x3 matrices
// No libm and no STL: every function is implemented here.
#pragma once
#include "math/scalar.hpp"
#include "math/vec.hpp"
#include "math/glsl.hpp"
#include "math/quat.hpp"
#include "math/mat4.hpp"
#include "math/mat23.hpp"
