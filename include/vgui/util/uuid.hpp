#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: UUID                                                   ║
// ║  128-bit random UUID v4. Zero external deps.                    ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <cstdint>
#include <string>
#include <array>

namespace vgui {

struct UUID {
    std::array<uint8_t, 16> bytes {};

    static UUID generate();              ///< Random v4 UUID
    static UUID from_string(const std::string& s);

    std::string to_string() const;       ///< "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"

    bool operator==(const UUID& o) const = default;
    bool operator!=(const UUID& o) const = default;
    bool operator< (const UUID& o) const;

    bool is_nil() const;
    static constexpr UUID nil() { return {}; }
};

} // namespace vgui

// ── std::hash specialisation ─────────────────────────────────────────
namespace std {
template<> struct hash<vgui::UUID> {
    size_t operator()(const vgui::UUID& id) const noexcept;
};
}
