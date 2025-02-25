#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>

#include "../include/chip8.h"
#include "../include/chip8keyboard.h"
#include "../include/config.h"

const char keyboard_map[CHIP8_TOTAL_KEYS] = {SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
                                             SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v};

int main(int argc, char **argv)
{
    // Test if memory setter and getter work
    struct chip8 chip8;
    chip8_keyboard_down(&chip8.keyboard, 0x0e);
    bool is_down = chip8_keyboard_is_down(&chip8.keyboard, 0x0f);
    printf("%i\n", is_down);

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

        switch (e.type)
        {
        case SDL_QUIT:
            quit = 1;
            break;

        case SDL_KEYDOWN: {
            char key = e.key.keysym.sym;
            int virt_key = chip8_keyboard_map(keyboard_map, key);
            printf("Key is down: %x\n", virt_key);
        }
        break;

        case SDL_KEYUP: {
            char key = e.key.keysym.sym;
            int virt_key = chip8_keyboard_map(keyboard_map, key);
            printf("Key is up: %x\n", virt_key);
        }
        break;
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
