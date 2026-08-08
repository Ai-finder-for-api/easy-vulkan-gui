#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace vgui {
namespace filesystem {

/**
 * @brief File system utilities
 */

/**
 * @brief Check if file exists
 */
inline bool exists(const std::string& path) {
    return std::filesystem::exists(path);
}

/**
 * @brief Check if path is a file
 */
inline bool isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

/**
 * @brief Check if path is a directory
 */
inline bool isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

/**
 * @brief Create directory
 */
inline bool createDirectory(const std::string& path, bool recursive = true) {
    std::error_code ec;
    if (recursive) {
        std::filesystem::create_directories(path, ec);
    } else {
        std::filesystem::create_directory(path, ec);
    }
    return !ec;
}

/**
 * @brief Remove file or directory
 */
inline bool remove(const std::string& path, bool recursive = false) {
    std::error_code ec;
    if (recursive) {
        std::filesystem::remove_all(path, ec);
    } else {
        std::filesystem::remove(path, ec);
    }
    return !ec;
}

/**
 * @brief Get file size
 */
inline size_t getFileSize(const std::string& path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    return ec ? 0 : static_cast<size_t>(size);
}

/**
 * @brief Read entire file into string
 */
inline std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    return content;
}

/**
 * @brief Read entire file into vector
 */
inline std::vector<uint8_t> readBinaryFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return buffer;
    }
    return {};
}

/**
 * @brief Write string to file
 */
inline bool writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    
    file.write(content.data(), content.size());
    return file.good();
}

/**
 * @brief Write binary data to file
 */
inline bool writeBinaryFile(const std::string& path, 
                           const void* data, size_t size) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    
    file.write(static_cast<const char*>(data), size);
    return file.good();
}

/**
 * @brief Get directory name
 */
inline std::string directory(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

/**
 * @brief Get file name
 */
inline std::string filename(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

/**
 * @brief Get file extension
 */
inline std::string extension(const std::string& path) {
    auto ext = std::filesystem::path(path).extension();
    return ext.string();
}

/**
 * @brief Get file stem (name without extension)
 */
inline std::string stem(const std::string& path) {
    return std::filesystem::path(path).stem().string();
}

/**
 * @brief Get absolute path
 */
inline std::string absolute(const std::string& path) {
    return std::filesystem::absolute(path).string();
}

/**
 * @brief Get relative path
 */
inline std::string relative(const std::string& from, const std::string& to) {
    return std::filesystem::relative(to, from).string();
}

/**
 * @brief Join paths
 */
inline std::string join(const std::string& a, const std::string& b) {
    return std::filesystem::path(a) / b;
}

/**
 * @brief Normalize path
 */
inline std::string normalize(const std::string& path) {
    return std::filesystem::path(path).lexically_normal().string();
}

/**
 * @brief Get current working directory
 */
inline std::string currentPath() {
    return std::filesystem::current_path().string();
}

/**
 * @brief Set current working directory
 */
inline bool setCurrentPath(const std::string& path) {
    std::error_code ec;
    std::filesystem::current_path(path, ec);
    return !ec;
}

/**
 * @brief List files in directory
 */
inline std::vector<std::string> listFiles(const std::string& directory,
                                          const std::string& pattern = "*") {
    std::vector<std::string> files;
    
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(
             directory, std::filesystem::directory_options::skip_permission_denied, ec)) {
        if (!ec && entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
    
    return files;
}

/**
 * @brief List directories
 */
inline std::vector<std::string> listDirectories(const std::string& directory) {
    std::vector<std::string> dirs;
    
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(
             directory, std::filesystem::directory_options::skip_permission_denied, ec)) {
        if (!ec && entry.is_directory()) {
            dirs.push_back(entry.path().string());
        }
    }
    
    return dirs;
}

/**
 * @brief Check if path is absolute
 */
inline bool isAbsolute(const std::string& path) {
    return std::filesystem::path(path).is_absolute();
}

/**
 * @brief Get file permissions
 */
inline bool isReadable(const std::string& path) {
    std::error_code ec;
    return std::filesystem::is_readable(path, ec) && !ec;
}

/**
 * @brief Get last modified time
 */
inline std::filesystem::file_time_type lastModified(const std::string& path) {
    std::error_code ec;
    return std::filesystem::last_write_time(path, ec);
}

/**
 * @brief Copy file
 */
inline bool copyFile(const std::string& from, const std::string& to,
                    bool overwrite = true) {
    std::error_code ec;
    std::filesystem::copy_file(from, to,
                              overwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none,
                              ec);
    return !ec;
}

/**
 * @brief Rename/move file
 */
inline bool rename(const std::string& from, const std::string& to) {
    std::error_code ec;
    std::filesystem::rename(from, to, ec);
    return !ec;
}

/**
 * @brief Get temp directory
 */
inline std::string tempDirectory() {
    return std::filesystem::temp_directory_path().string();
}

/**
 * @brief Create temp file
 */
inline std::string createTempFile(const std::string& prefix = "vgui_",
                                  const std::string& suffix = ".tmp") {
    static int counter = 0;
    std::string filename = prefix + std::to_string(counter++) + suffix;
    std::string path = std::filesystem::temp_directory_path() / filename;
    
    std::ofstream file(path);
    file.close();
    
    return path.string();
}

/**
 * @brief Get application directory
 */
inline std::string applicationDirectory() {
    // This should be set by the application
    static std::string appDir;
    return appDir;
}

/**
 * @brief Set application directory
 */
inline void setApplicationDirectory(const std::string& path) {
    static std::string appDir;
    appDir = path;
}

/**
 * @brief Get data directory
 */
inline std::string dataDirectory() {
    // Usually applicationDirectory() / "data"
    auto appDir = applicationDirectory();
    if (!appDir.empty()) {
        return (std::filesystem::path(appDir) / "data").string();
    }
    return appDir;
}

} // namespace filesystem
} // namespace vgui
