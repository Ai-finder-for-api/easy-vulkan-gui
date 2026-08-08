#pragma once

#include <string>
#include <array>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <cstring>

namespace vgui {

/**
 * @brief UUID (Universally Unique Identifier)
 * 
 * RFC 4122 compliant UUID generation and manipulation.
 */
class UUID {
public:
    /**
     * @brief Create null UUID
     */
    UUID();
    
    /**
     * @brief Create from string
     */
    explicit UUID(const std::string& str);
    
    /**
     * @brief Create from bytes
     */
    explicit UUID(const std::array<uint8_t, 16>& bytes);
    
    /**
     * @brief Create random UUID (v4)
     */
    static UUID random();
    
    /**
     * @brief Create from raw bytes (big-endian)
     */
    static UUID fromBytes(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3,
                         uint8_t b4, uint8_t b5, uint8_t b6, uint8_t b7,
                         uint8_t b8, uint8_t b9, uint8_t b10, uint8_t b11,
                         uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
    
    /**
     * @brief Destroy
     */
    ~UUID() = default;
    
    /**
     * @brief Check if null
     */
    bool isNull() const;
    
    /**
     * @brief Get as string
     */
    std::string toString() const;
    
    /**
     * @brief Get as hyphenated string
     */
    std::string toHyphenatedString() const;
    
    /**
     * @brief Get as bytes
     */
    std::array<uint8_t, 16> toBytes() const;
    
    /**
     * @brief Get variant
     */
    uint8_t getVariant() const;
    
    /**
     * @brief Get version
     */
    uint8_t getVersion() const;
    
    /**
     * @brief Set variant
     */
    void setVariant(uint8_t variant);
    
    /**
     * @brief Set version
     */
    void setVersion(uint8_t version);
    
    /**
     * @brief Compare UUIDs
     */
    bool operator==(const UUID& other) const;
    bool operator!=(const UUID& other) const;
    bool operator<(const UUID& other) const;
    
    /**
     * @brief Hash
     */
    size_t hash() const;
    
    /**
     * @brief Convert to/from string
     */
    static UUID fromString(const std::string& str);
    
    /**
     * @brief Create namespace UUIDs (RFC 4122)
     */
    static UUID dnsNamespace();
    static UUID urlNamespace();
    static UUID oidNamespace();
    static UUID x500Namespace();
    
private:
    std::array<uint8_t, 16> data_;
    
    void fromStringImpl(const std::string& str);
    static UUID generateRandom();
    static UUID generateTimeBased();
};

/**
 * @brief UUID generator
 */
class UUIDGenerator {
public:
    /**
     * @brief Generate random UUID (v4)
     */
    static UUID generateUUID4();
    
    /**
     * @brief Generate time-based UUID (v1)
     */
    static UUID generateUUID1();
    
    /**
     * @brief Generate name-based UUID (v3 - MD5)
     */
    static UUID generateUUID3(const UUID& namespaceUUID, 
                             const std::string& name);
    
    /**
     * @brief Generate name-based UUID (v5 - SHA-1)
     */
    static UUID generateUUID5(const UUID& namespaceUUID,
                             const std::string& name);
    
    /**
     * @brief Get random number generator
     */
    static std::mt19937& getRandomEngine();
    
private:
    static std::mt19937 rng_;
    static bool initialized_;
    
    UUIDGenerator() = delete;
};

/**
 * @brief Stream output for UUID
 */
inline std::ostream& operator<<(std::ostream& os, const UUID& uuid) {
    os << uuid.toHyphenatedString();
    return os;
}

/**
 * @brief Stream input for UUID
 */
inline std::istream& operator>>(std::istream& is, UUID& uuid) {
    std::string str;
    is >> str;
    uuid = UUID::fromString(str);
    return is;
}

/**
 * @brief Hash specialization
 */
template<>
struct std::hash<UUID> {
    size_t operator()(const UUID& uuid) const {
        return uuid.hash();
    }
};

} // namespace vgui
