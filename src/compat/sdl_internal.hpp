// Internal helpers shared by the SDL2 shim files (src/compat/sdl_*.cpp). Not part of the public API.
#pragma once
#include <evg/evg.hpp>
#include <evg/compat/SDL.h>

namespace evgx { namespace sdl {

// key tables: evg key code (uppercase ASCII / key::*) <-> SDL scancode and keycode
int scancode_of(int vk);
int keycode_of(int vk);

// keyboard state buffer returned by SDL_GetKeyboardState (defined in sdl_keys.cpp)
extern Uint8 g_keystate[SDL_NUM_SCANCODES];

// relative mouse mode flag (defined in sdl_input.cpp)
extern bool g_relative;

// timing base: SDL counts ticks from SDL_Init or the first timing call (defined in sdl_time.cpp)
double since_start();

// the window that mouse queries refer to: the one that most recently produced input, else the first window
Window* mouse_window();

}}  // namespace evgx::sdl
