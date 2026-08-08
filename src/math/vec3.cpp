#include <vgui/math/vec3.hpp>
#include <cstdio>

namespace vgui {
std::string Vec3::to_string() const {
    char buf[96];
    std::snprintf(buf, sizeof(buf), "Vec3(%.4f, %.4f, %.4f)", x, y, z);
    return buf;
}
} // namespace vgui
