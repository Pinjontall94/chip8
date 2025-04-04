#include "../include/chip8.h"
#include "../include/chip8keyboard.h"
#include "../include/config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_AudioDeviceID dev;
    SDL_AudioSpec want, obtained;
} sdl_t;

bool sdl_setup(sdl_t *sdl);
void audio_callback(void *userdata, Uint8 *stream, int stream_len);
void square_oscillator(Sint16 *stream, int stream_len, int freq, double amp);

const char keyboard_map[CHIP8_TOTAL_KEYS] = {SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
                                             SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v};

int main(void)
{
    // Final return code
    int status = 0;
    // Test if memory setter and getter work
    struct chip8 chip8;
    chip8_init(&chip8);
    chip8.registers.sound_timer = 5;

    for (int i = 0; i < 9; i++)
    {
        chip8_screen_draw_sprite(&chip8.screen, i + (i * 5), i, &chip8.memory.memory[(i * 5)], 5);
    }

    sdl_t sdl = {0};
    if (!sdl_setup(&sdl))
        return 1;

    sdl.window = SDL_CreateWindow(EMULATOR_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER, CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER,
                                          SDL_WINDOW_SHOWN);

    sdl.renderer = SDL_CreateRenderer(sdl.window, -1, SDL_TEXTUREACCESS_TARGET);

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
            if (virt_key != -1)
            {
                chip8_keyboard_down(&chip8.keyboard, virt_key);
            }
        }
        break;

        case SDL_KEYUP: {
            char key = e.key.keysym.sym;
            int virt_key = chip8_keyboard_map(keyboard_map, key);
            if (virt_key != -1)
            {
                chip8_keyboard_up(&chip8.keyboard, virt_key);
            }
        }
        break;
        }

        // Paint Screen Black
        SDL_SetRenderDrawColor(sdl.renderer, 0, 0, 0, 0);
        SDL_RenderClear(sdl.renderer);
        SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 255, 0);

        // Loop through screen and paint pixels
        for (int x = 0; x < CHIP8_WIDTH; x++)
        {
            for (int y = 0; y < CHIP8_HEIGHT; y++)
            {
                if (chip8_screen_is_set(&chip8.screen, x, y))
                {
                    SDL_Rect r;
                    r.x = x * CHIP8_WINDOW_MULTIPLIER;
                    r.y = y * CHIP8_WINDOW_MULTIPLIER;
                    r.w = CHIP8_WINDOW_MULTIPLIER;
                    r.h = CHIP8_WINDOW_MULTIPLIER;
                    SDL_RenderFillRect(sdl.renderer, &r);
                }
            }
        }

        SDL_RenderPresent(sdl.renderer);

        if (chip8.registers.delay_timer > 0)
        {
            SDL_Delay(100);
            chip8.registers.delay_timer--;
        }

        if (chip8.registers.sound_timer > 0)
        {
            SDL_Delay(100);
            // Fill the samples buffer with a square wave
            SDL_PauseAudioDevice(sdl.dev, 0);
            chip8.registers.sound_timer--;
        }
        else
        {
            SDL_PauseAudioDevice(sdl.dev, 1);
        }
    }
    SDL_DestroyRenderer(sdl.renderer);
    SDL_DestroyWindow(sdl.window);
    return status;
}

bool sdl_setup(sdl_t *sdl)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fprintf(stderr, "Failed to init subsystem: %s\n", SDL_GetError());
        return false;
    }

    sdl->want = (SDL_AudioSpec){
        .freq = SAMPLE_RATE,
        .format = AUDIO_S16LSB, // Sint16, little-endian
        .channels = 1,
        .size = AUDIO_BUF_SIZE,
        .callback = audio_callback,
    };
    sdl->obtained = (SDL_AudioSpec){0};

    sdl->dev = SDL_OpenAudioDevice(NULL, 0, &sdl->want, &sdl->obtained, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (sdl->dev < 1)
    {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        return false;
    }
    // Unpause audio (start playback)
    SDL_PauseAudioDevice(sdl->dev, 0);
    printf("Successfully initialized audio\n");
    return true;
}

void audio_callback(void *userdata, Uint8 *stream, int stream_len)
{
    (void)userdata;
    assert((stream_len & (stream_len - 1)) == 0); // assert length is a power of 2
    // Convert types and pass stream to oscillator
    square_oscillator((Sint16 *)stream, stream_len / 2, 512, 0.05);
}

void square_oscillator(Sint16 *stream, int stream_len, int freq, double amp)
{
    // Make sure freq is below nyquist and volume isn't too loud [WARNING: DO NOT USE HEADPHONES]
    Sint16 const MAX = floor(65535.0 / 2.0);
    float const delta = (float)freq / SAMPLE_RATE;
    double phase = 0.00;
    Sint16 value = 0;

    assert(freq < (SAMPLE_RATE / 2) && amp > 0.00 && amp < 0.1);
    for (int i = 0; i < stream_len; i++)
    {
        if (phase < 0.5)
        {
            value = MAX;
        }
        else
        {
            value = -1 * MAX;
        }
        Sint16 final_value = (Sint16)(value * amp);
        phase += delta; // heart of saw wave: linearly track delta as phase increases
        if (phase >= 1)
            phase -= 1;
        stream[i] = final_value;
    }
}
