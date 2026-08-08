#include <vgui/math/color.hpp>
#include <cstdio>

namespace vgui {
std::string Color::to_string() const {
    char buf[128];
    std::snprintf(buf, sizeof(buf), "Color(r=%.3f, g=%.3f, b=%.3f, a=%.3f)", r, g, b, a);
    return buf;
}
} // namespace vgui
