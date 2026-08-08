// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: UUID  –  implementation  (no external deps)            ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/util/uuid.hpp>
#include <cstdio>
#include <cstring>
#include <random>
#include <mutex>

namespace vgui {

static std::mt19937_64& rng() {
    static std::mutex              mtx;
    static std::random_device      rd;
    static std::mt19937_64         eng(rd());
    return eng;
}

UUID UUID::generate() {
    static std::mutex mtx;
    UUID id;
    {
        std::lock_guard lock(mtx);
        auto& e = rng();
        uint64_t hi = e(), lo = e();
        std::memcpy(id.bytes.data(),     &hi, 8);
        std::memcpy(id.bytes.data() + 8, &lo, 8);
    }
    // Set version 4 and variant bits
    id.bytes[6] = (id.bytes[6] & 0x0F) | 0x40;
    id.bytes[8] = (id.bytes[8] & 0x3F) | 0x80;
    return id;
}

std::string UUID::to_string() const {
    char buf[37];
    std::snprintf(buf, sizeof(buf),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        bytes[0],bytes[1],bytes[2],bytes[3],
        bytes[4],bytes[5], bytes[6],bytes[7],
        bytes[8],bytes[9], bytes[10],bytes[11],
        bytes[12],bytes[13],bytes[14],bytes[15]);
    return buf;
}

UUID UUID::from_string(const std::string& s) {
    UUID id;
    if (s.size() < 32) return id;
    std::string clean;
    for (char c : s) if (c != '-') clean += c;
    for (int i = 0; i < 16; ++i) {
        unsigned v = 0;
        std::sscanf(clean.c_str() + i*2, "%02x", &v);
        id.bytes[i] = static_cast<uint8_t>(v);
    }
    return id;
}

bool UUID::is_nil() const {
    for (auto b : bytes) if (b) return false;
    return true;
}

bool UUID::operator<(const UUID& o) const {
    return bytes < o.bytes;
}

} // namespace vgui

namespace std {
size_t hash<vgui::UUID>::operator()(const vgui::UUID& id) const noexcept {
    size_t h = 0;
    for (auto b : id.bytes) h = h * 31 + b;
    return h;
}
}
