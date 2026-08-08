// ╔══════════════════════════════════════════════════════════════════╗
// ║  vgui :: Filesystem  –  implementation  (no external deps)      ║
// ╚══════════════════════════════════════════════════════════════════╝

#include <vgui/util/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

#if defined(_WIN32)
#  include <windows.h>
#  include <shlobj.h>
#elif defined(__APPLE__)
#  include <mach-o/dyld.h>
#  include <sys/stat.h>
#  include <climits>
#else
#  include <unistd.h>
#  include <sys/stat.h>
#  include <climits>
#endif

namespace vgui::fs {

namespace stdfs = std::filesystem;

// ── Read / write ──────────────────────────────────────────────────
std::vector<uint8_t> read_bytes(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    return { std::istreambuf_iterator<char>(f), {} };
}

std::string read_text(const std::string& path) {
    std::ifstream f(path);
    if (!f) return {};
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

bool write_bytes(const std::string& path, const void* data, size_t size) {
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    f.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
    return f.good();
}

bool write_text(const std::string& path, const std::string& text) {
    std::ofstream f(path);
    if (!f) return false;
    f << text;
    return f.good();
}

// ── Path helpers ──────────────────────────────────────────────────
bool        exists      (const std::string& p) { return stdfs::exists(p);         }
bool        is_file     (const std::string& p) { return stdfs::is_regular_file(p);}
bool        is_directory(const std::string& p) { return stdfs::is_directory(p);   }
std::string extension   (const std::string& p) { return stdfs::path(p).extension().string();        }
std::string filename    (const std::string& p) { return stdfs::path(p).filename().string();         }
std::string stem        (const std::string& p) { return stdfs::path(p).stem().string();             }
std::string parent_path (const std::string& p) { return stdfs::path(p).parent_path().string();      }
std::string absolute    (const std::string& p) { return stdfs::absolute(p).string();                }
std::string normalize   (const std::string& p) { return stdfs::weakly_canonical(p).string();        }
std::string join        (const std::string& a, const std::string& b) {
    return (stdfs::path(a) / b).string();
}

bool mkdir(const std::string& p)  { return stdfs::create_directories(p); }
bool remove(const std::string& p) { return stdfs::remove_all(p) > 0;     }

std::vector<std::string> list_dir(const std::string& path) {
    std::vector<std::string> r;
    for (auto& e : stdfs::directory_iterator(path))
        r.push_back(e.path().string());
    return r;
}

std::vector<std::string> list_dir_ext(const std::string& path, const std::string& ext) {
    std::vector<std::string> r;
    for (auto& e : stdfs::directory_iterator(path))
        if (e.path().extension() == ext) r.push_back(e.path().string());
    return r;
}

// ── Application paths ─────────────────────────────────────────────
std::string exe_dir() {
#if defined(_WIN32)
    wchar_t buf[MAX_PATH]; GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return stdfs::path(buf).parent_path().string();
#elif defined(__APPLE__)
    char buf[PATH_MAX]; uint32_t sz = PATH_MAX;
    _NSGetExecutablePath(buf, &sz);
    return stdfs::path(buf).parent_path().string();
#else
    char buf[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", buf, PATH_MAX);
    if (n < 0) return ".";
    buf[n] = '\0';
    return stdfs::path(buf).parent_path().string();
#endif
}

std::string data_dir() {
#if defined(_WIN32)
    wchar_t buf[MAX_PATH]; SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, buf);
    return stdfs::path(buf).string();
#elif defined(__APPLE__)
    return join(std::string(getenv("HOME")), "Library/Application Support");
#else
    const char* xdg = getenv("XDG_DATA_HOME");
    return xdg ? xdg : join(std::string(getenv("HOME")), ".local/share");
#endif
}

std::string cache_dir() {
#if defined(_WIN32)
    wchar_t buf[MAX_PATH]; SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, buf);
    return stdfs::path(buf).string();
#elif defined(__APPLE__)
    return join(std::string(getenv("HOME")), "Library/Caches");
#else
    const char* xdg = getenv("XDG_CACHE_HOME");
    return xdg ? xdg : join(std::string(getenv("HOME")), ".cache");
#endif
}

std::string temp_dir() { return stdfs::temp_directory_path().string(); }

} // namespace vgui::fs
