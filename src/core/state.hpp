// Internal state shared by the core .cpp files. Not part of the public API.
#pragma once
#include <stdint.h>
#include <evg/evg.hpp>
#include <evg/vk_min.h>
#include "raster.hpp"
#include <evg/lighting.hpp>

namespace evgx { namespace st {

constexpr int kMaxWindows = 16;
constexpr int kMaxModels = 64;
constexpr int kQueue = 256;          // event ring buffer; new events are dropped when full

extern Window g_pool[kMaxWindows];
extern bool   g_used[kMaxWindows];
extern Model* g_models[kMaxModels];
extern char   g_err[256];
extern char   g_gpu[256];
extern bool   g_inited, g_init_ok;

extern vk::PFN_GetInstanceProcAddr g_gipa;
extern vk::Instance g_instance;

extern Window*     g_cur;            // window of the current software frame (begin .. end)
extern FrameState  g_fs;
extern bool        g_have_cam;

extern Event   g_q[kQueue];
extern int     g_qh, g_qn;
extern Window* g_focus;              // window that most recently produced an event

extern Lighting g_env;
extern bool     g_env_on;

// copies a C string into a fixed buffer, always terminated
inline void copy(char* dst, const char* src, int cap) {
    int i = 0;
    for (; src && src[i] && i < cap - 1; i++) dst[i] = src[i];
    dst[i] = 0;
}
// records an error message for evg.error() and returns false
inline bool fail(const char* msg) { copy(g_err, msg, sizeof g_err); return false; }
inline void put32(unsigned char* p, uint32_t v) {
    p[0] = (unsigned char)v; p[1] = (unsigned char)(v >> 8); p[2] = (unsigned char)(v >> 16); p[3] = (unsigned char)(v >> 24);
}

void push_event(const Event& e);     // appends to the queue and updates the focus window
void fb_free(Window* w);             // releases a window's software framebuffer
bool fb_ensure(Window* w);           // (re)allocates the framebuffer to the window's size

}}  // namespace evgx::st
