#pragma once

// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Filesystem                                             ║
// ║  Thin cross-platform file utilities. Zero external deps.        ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace vgui::fs {

// ── Read / write ──────────────────────────────────────────────────
std::vector<uint8_t> read_bytes(const std::string& path);
std::string          read_text (const std::string& path);
bool write_bytes(const std::string& path, const void* data, size_t size);
bool write_text (const std::string& path, const std::string& text);

// ── Path helpers ──────────────────────────────────────────────────
bool        exists       (const std::string& path);
bool        is_file      (const std::string& path);
bool        is_directory (const std::string& path);
std::string extension    (const std::string& path);   ///< ".png"
std::string filename     (const std::string& path);   ///< "icon.png"
std::string stem         (const std::string& path);   ///< "icon"
std::string parent_path  (const std::string& path);
std::string join         (const std::string& a, const std::string& b);
std::string absolute     (const std::string& path);
std::string normalize    (const std::string& path);   ///< Resolve ..

// ── Directory ─────────────────────────────────────────────────────
bool mkdir  (const std::string& path);
bool remove (const std::string& path);
std::vector<std::string> list_dir     (const std::string& path);
std::vector<std::string> list_dir_ext (const std::string& path, const std::string& ext);

// ── Application paths ─────────────────────────────────────────────
std::string exe_dir   ();   ///< Directory containing the executable
std::string data_dir  ();   ///< Platform user-data directory
std::string cache_dir ();   ///< Platform cache directory
std::string temp_dir  ();

} // namespace vgui::fs
