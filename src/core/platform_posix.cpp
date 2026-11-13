#if !defined(_WIN32)
#include <dlfcn.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include "platform.hpp"

namespace evgx { namespace plat {
void* load_vulkan() {
    void* h = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (!h) h = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    return h ? dlsym(h, "vkGetInstanceProcAddr") : nullptr;
}
// Headless window: no OS window, but the software framebuffer works (screenshots, tests, servers).
bool window_create(Window*, int, int, const char*, const char**) { return true; }
void window_destroy(Window*) {}
void present(Window*) {}
void* app_instance() { return nullptr; }
void prepare_external(void*) {}
void pump() {}
void sleep_ms(int ms) { usleep(static_cast<useconds_t>(ms) * 1000); }
double time_seconds() { timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9; }
#ifdef EVG_USE_LIBC_ALLOC   // test builds only: lets AddressSanitizer see every allocation
}} 
#include <stdlib.h>
namespace evgx { namespace plat {
void* raw_alloc(size_t n) { return calloc(1, n); }
void raw_free(void* p, size_t) { free(p); }
#else
void* raw_alloc(size_t n) {
    void* p = mmap(nullptr, n, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return p == MAP_FAILED ? nullptr : p;
}
void raw_free(void* p, size_t n) { if (p) munmap(p, n); }
#endif
long long file_size(const char* path) {
    int fd = open(path, O_RDONLY); if (fd < 0) return -1;
    off_t e = lseek(fd, 0, SEEK_END); close(fd);
    return e < 0 ? -1 : (long long)e;
}
bool file_read(const char* path, void* dst, size_t n) {
    int fd = open(path, O_RDONLY); if (fd < 0) return false;
    size_t got = 0;
    while (got < n) { ssize_t r = read(fd, (char*)dst + got, n - got); if (r <= 0) break; got += (size_t)r; }
    close(fd);
    return got == n;
}
bool file_write(const char* path, const void* src, size_t n) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644); if (fd < 0) return false;
    size_t put = 0;
    while (put < n) { ssize_t r = write(fd, (const char*)src + put, n - put); if (r <= 0) break; put += (size_t)r; }
    close(fd);
    return put == n;
}
}}
#endif
