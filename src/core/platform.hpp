#pragma once
#include <stddef.h>
#include <evg/evg.hpp>
namespace evgx { namespace plat {
void* load_vulkan();                                            // vkGetInstanceProcAddr or nullptr
bool  window_create(Window* w, int width, int height, const char* title, const char** err);
void  window_destroy(Window* w);
void  present(Window* w);                                       // blit w->color (fbw x fbh); no-op when headless
void* app_instance();                                           // HINSTANCE on Windows, null elsewhere
void  prepare_external(void* native);                           // make a host-owned window safe for presentation
void  pump();
void  sleep_ms(int ms);
double time_seconds();
void* raw_alloc(size_t n);                                      // zeroed
void  raw_free(void* p, size_t n);
long long file_size(const char* path);                          // -1 on error
bool  file_read(const char* path, void* dst, size_t n);
bool  file_write(const char* path, const void* src, size_t n);
}}
