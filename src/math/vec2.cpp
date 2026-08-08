#include <vgui/math/vec2.hpp>
#include <cstdio>

namespace vgui {
std::string Vec2::to_string() const {
    char buf[64];
    std::snprintf(buf, sizeof(buf), "Vec2(%.4f, %.4f)", x, y);
    return buf;
}
} // namespace vgui
