#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: version                                                ║
// ║  Compile-time version constants.                                ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string_view>
#include <cstdint>

namespace vgui {

inline constexpr uint32_t     VERSION_MAJOR  = 0;
inline constexpr uint32_t     VERSION_MINOR  = 1;
inline constexpr uint32_t     VERSION_PATCH  = 0;
inline constexpr std::string_view VERSION_STRING = "0.1.0";

/// Packed as 0xMMMMNNPP  (major · minor · patch)
inline constexpr uint32_t VERSION_PACKED =
    (VERSION_MAJOR << 16) | (VERSION_MINOR << 8) | VERSION_PATCH;

} // namespace vgui
