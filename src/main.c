#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window =
        SDL_CreateWindow("Chip8 Window", SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);

    printf("hey girl! :3\n");

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

    // Quit Condition Flag
    int quit = 0;

    // Event Loop
    while (quit == 0) {
        SDL_Event e;
        SDL_WaitEvent(&e);

        if (e.type == SDL_QUIT) {
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
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
