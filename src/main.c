#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

#include "../include/chip8.h"

int main(int argc, char **argv)
{
    // Test if memory setter and getter work
    struct chip8 chip8;
    chip8_memory_set(&chip8.memory, 0x50, 'Z');
    printf("%c\n", chip8_memory_get(&chip8.memory, 0x50));

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(EMULATOR_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER, CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

    // Quit Condition Flag
    int quit = 0;

    // Event Loop
    while (quit == 0)
    {
        SDL_Event e;
        SDL_WaitEvent(&e);

        if (e.type == SDL_QUIT)
        {
            quit = 1;
        }

        // Paint Screen Black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        SDL_Rect r;
        r.x = 0;
        r.y = 0;
        r.w = 40;
        r.h = 40;
        SDL_RenderDrawRect(renderer, &r);
        SDL_RenderFillRect(renderer, &r);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
