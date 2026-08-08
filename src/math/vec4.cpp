#include <vgui/math/vec4.hpp>
#include <cstdio>

namespace vgui {
std::string Vec4::to_string() const {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Vec4(%.4f, %.4f, %.4f, %.4f)", x, y, z, w);
    return buf;
}
} // namespace vgui
