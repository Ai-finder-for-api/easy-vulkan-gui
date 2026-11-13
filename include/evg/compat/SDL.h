// SDL2-compatible shim backed by evg. Put this directory before the real SDL2 include path.
// Covers: init/window basics, the event queue (keyboard, mouse, quit, resize), keyboard/mouse state, timing.
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef uint8_t  Uint8;  typedef uint16_t Uint16; typedef uint32_t Uint32; typedef uint64_t Uint64;
typedef int8_t   Sint8;  typedef int16_t  Sint16; typedef int32_t  Sint32; typedef int64_t  Sint64;
typedef Sint32 SDL_Keycode;
typedef Sint32 SDL_Scancode;   // enum in real SDL; plain int here (values match)
typedef enum SDL_bool { SDL_FALSE = 0, SDL_TRUE = 1 } SDL_bool;
typedef struct SDL_Window SDL_Window;

#define SDL_INIT_VIDEO 0x00000020u
#define SDL_INIT_EVENTS 0x00004000u
#define SDL_WINDOW_SHOWN 0x00000004u
#define SDL_WINDOW_RESIZABLE 0x00000020u
#define SDL_WINDOW_VULKAN 0x10000000u
#define SDL_WINDOWPOS_UNDEFINED 0x1FFF0000
#define SDL_WINDOWPOS_CENTERED 0x2FFF0000

/* event types (values as in SDL2) */
#define SDL_QUIT 0x100u
#define SDL_WINDOWEVENT 0x200u
#define SDL_KEYDOWN 0x300u
#define SDL_KEYUP 0x301u
#define SDL_MOUSEMOTION 0x400u
#define SDL_MOUSEBUTTONDOWN 0x401u
#define SDL_MOUSEBUTTONUP 0x402u
#define SDL_MOUSEWHEEL 0x403u
#define SDL_WINDOWEVENT_SIZE_CHANGED 6
#define SDL_PRESSED 1
#define SDL_RELEASED 0

#define SDL_BUTTON_LEFT 1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT 3
#define SDL_BUTTON_LMASK (1u << 0)
#define SDL_BUTTON_MMASK (1u << 1)
#define SDL_BUTTON_RMASK (1u << 2)
#define SDL_BUTTON(b) (1u << ((b) - 1))

#define SDL_NUM_SCANCODES 512
/* a subset of SDL_Scancode values (same numbers as SDL2) */
enum {
    SDL_SCANCODE_UNKNOWN = 0,
    SDL_SCANCODE_A = 4, SDL_SCANCODE_B, SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E, SDL_SCANCODE_F, SDL_SCANCODE_G,
    SDL_SCANCODE_H, SDL_SCANCODE_I, SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_L, SDL_SCANCODE_M, SDL_SCANCODE_N,
    SDL_SCANCODE_O, SDL_SCANCODE_P, SDL_SCANCODE_Q, SDL_SCANCODE_R, SDL_SCANCODE_S, SDL_SCANCODE_T, SDL_SCANCODE_U,
    SDL_SCANCODE_V, SDL_SCANCODE_W, SDL_SCANCODE_X, SDL_SCANCODE_Y, SDL_SCANCODE_Z,
    SDL_SCANCODE_1 = 30, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7,
    SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_0,
    SDL_SCANCODE_RETURN = 40, SDL_SCANCODE_ESCAPE, SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_TAB, SDL_SCANCODE_SPACE,
    SDL_SCANCODE_RIGHT = 79, SDL_SCANCODE_LEFT, SDL_SCANCODE_DOWN, SDL_SCANCODE_UP,
    SDL_SCANCODE_LCTRL = 224, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_LALT
};
#define SDLK_SCANCODE_MASK (1 << 30)
#define SDLK_RETURN 13
#define SDLK_ESCAPE 27
#define SDLK_BACKSPACE 8
#define SDLK_TAB 9
#define SDLK_SPACE 32
#define SDLK_RIGHT (SDLK_SCANCODE_MASK | SDL_SCANCODE_RIGHT)
#define SDLK_LEFT (SDLK_SCANCODE_MASK | SDL_SCANCODE_LEFT)
#define SDLK_DOWN (SDLK_SCANCODE_MASK | SDL_SCANCODE_DOWN)
#define SDLK_UP (SDLK_SCANCODE_MASK | SDL_SCANCODE_UP)
#define SDLK_LCTRL (SDLK_SCANCODE_MASK | SDL_SCANCODE_LCTRL)
#define SDLK_LSHIFT (SDLK_SCANCODE_MASK | SDL_SCANCODE_LSHIFT)
#define SDLK_LALT (SDLK_SCANCODE_MASK | SDL_SCANCODE_LALT)
/* letters and digits: lowercase ASCII / digit characters, as in SDL2 */
#define SDLK_a 'a'
#define SDLK_b 'b'
#define SDLK_c 'c'
#define SDLK_d 'd'
#define SDLK_e 'e'
#define SDLK_f 'f'
#define SDLK_g 'g'
#define SDLK_h 'h'
#define SDLK_i 'i'
#define SDLK_j 'j'
#define SDLK_k 'k'
#define SDLK_l 'l'
#define SDLK_m 'm'
#define SDLK_n 'n'
#define SDLK_o 'o'
#define SDLK_p 'p'
#define SDLK_q 'q'
#define SDLK_r 'r'
#define SDLK_s 's'
#define SDLK_t 't'
#define SDLK_u 'u'
#define SDLK_v 'v'
#define SDLK_w 'w'
#define SDLK_x 'x'
#define SDLK_y 'y'
#define SDLK_z 'z'

typedef struct SDL_Keysym { SDL_Scancode scancode; SDL_Keycode sym; Uint16 mod; Uint32 unused; } SDL_Keysym;
typedef struct SDL_CommonEvent { Uint32 type; Uint32 timestamp; } SDL_CommonEvent;
typedef struct SDL_WindowEvent { Uint32 type; Uint32 timestamp; Uint32 windowID; Uint8 event; Uint8 padding1, padding2, padding3; Sint32 data1; Sint32 data2; } SDL_WindowEvent;
typedef struct SDL_KeyboardEvent { Uint32 type; Uint32 timestamp; Uint32 windowID; Uint8 state; Uint8 repeat; Uint8 padding2, padding3; SDL_Keysym keysym; } SDL_KeyboardEvent;
typedef struct SDL_MouseMotionEvent { Uint32 type; Uint32 timestamp; Uint32 windowID; Uint32 which; Uint32 state; Sint32 x, y, xrel, yrel; } SDL_MouseMotionEvent;
typedef struct SDL_MouseButtonEvent { Uint32 type; Uint32 timestamp; Uint32 windowID; Uint32 which; Uint8 button; Uint8 state; Uint8 clicks; Uint8 padding1; Sint32 x, y; } SDL_MouseButtonEvent;
typedef struct SDL_MouseWheelEvent { Uint32 type; Uint32 timestamp; Uint32 windowID; Uint32 which; Sint32 x, y; Uint32 direction; } SDL_MouseWheelEvent;
typedef struct SDL_QuitEvent { Uint32 type; Uint32 timestamp; } SDL_QuitEvent;
typedef union SDL_Event {
    Uint32 type;
    SDL_CommonEvent common;
    SDL_WindowEvent window;
    SDL_KeyboardEvent key;
    SDL_MouseMotionEvent motion;
    SDL_MouseButtonEvent button;
    SDL_MouseWheelEvent wheel;
    SDL_QuitEvent quit;
    Uint8 padding[56];
} SDL_Event;

/* core */
int  SDL_Init(Uint32 flags);
void SDL_Quit(void);
const char* SDL_GetError(void);
SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, Uint32 flags);
void SDL_DestroyWindow(SDL_Window* w);
void SDL_GetWindowSize(SDL_Window* w, int* width, int* height);
void SDL_SetWindowTitle(SDL_Window* w, const char* title);
void SDL_Delay(Uint32 ms);

/* events */
int  SDL_PollEvent(SDL_Event* e);
SDL_bool SDL_IsTextInputActive(void);
int  SDL_SetRelativeMouseMode(SDL_bool enabled);
SDL_bool SDL_GetRelativeMouseMode(void);
int  SDL_ShowCursor(int toggle);

/* keyboard / mouse state */
const Uint8* SDL_GetKeyboardState(int* numkeys);
Uint32 SDL_GetMouseState(int* x, int* y);
Uint32 SDL_GetRelativeMouseState(int* x, int* y);

/* timing */
Uint32 SDL_GetTicks(void);
Uint64 SDL_GetPerformanceCounter(void);
Uint64 SDL_GetPerformanceFrequency(void);
#ifdef __cplusplus
}
#endif
