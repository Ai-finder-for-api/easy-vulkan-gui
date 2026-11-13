// ex_sdl_loop: the classic SDL2 main loop running on evg: poll events, read the keyboard, draw, quit on Escape or after 3 seconds.
#include <stdio.h>
#include <evg/compat/SDL.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* win = SDL_CreateWindow("sdl loop", 0, 0, 320, 240, 0);
    if (!win) { printf("window: %s\n", SDL_GetError()); return 1; }

    Uint32 start = SDL_GetTicks();
    int keys = 0, clicks = 0, running = 1;
    while (running && SDL_GetTicks() - start < 3000) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                keys++;
                if (e.key.keysym.sym == SDLK_ESCAPE) running = 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) clicks++;
        }
        const Uint8* state = SDL_GetKeyboardState(nullptr);
        int rx = 0, ry = 0;
        SDL_GetRelativeMouseState(&rx, &ry);
        (void)state; (void)rx; (void)ry;
        SDL_Delay(16);
    }
    printf("key presses %d, clicks %d, ran %u ms\n", keys, clicks, (unsigned)(SDL_GetTicks() - start));
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
