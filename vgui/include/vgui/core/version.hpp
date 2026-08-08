#pragma once

#include <string>
#include <cstdint>

namespace vgui {

/**
 * @brief Version information for VGUI
 */
struct Version {
    uint32_t major = 1;
    uint32_t minor = 0;
    uint32_t patch = 0;

    /**
     * @brief Get the version string
     * @return Version in "major.minor.patch" format
     */
    std::string toString() const {
        return std::to_string(major) + "." + 
               std::to_string(minor) + "." + 
               std::to_string(patch);
    }

    /**
     * @brief Compare versions
     * @param other Version to compare against
     * @return true if this version is greater than other
     */
    bool operator>(const Version& other) const {
        if (major != other.major) return major > other.major;
        if (minor != other.minor) return minor > other.minor;
        return patch > other.patch;
    }

    /**
     * @brief Compare versions
     * @param other Version to compare against
     * @return true if this version is less than other
     */
    bool operator<(const Version& other) const {
        return other > *this;
    }

    /**
     * @brief Check equality
     */
    bool operator==(const Version& other) const {
        return major == other.major && 
               minor == other.minor && 
               patch == other.patch;
    }
};

// Compile-time version
constexpr Version VGUI_VERSION{1, 0, 0};

} // namespace vgui
