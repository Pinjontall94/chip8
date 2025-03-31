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

void fill_wave_table(Sint16 *samples, int samples_len, bool sound_on);
void pulse_wave(Sint16 *samples, int samples_len, Sint16 freq, double pulse_width, double vol);
void print_wave_table(Sint16 *samples, int samples_len);

const char keyboard_map[CHIP8_TOTAL_KEYS] = {SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
                                             SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v};

int main(void)
{
    // Final return code
    int status = 0;
    // Test if memory setter and getter work
    struct chip8 chip8;
    chip8_init(&chip8);
    chip8.registers.sound_timer = 20;

    for (int i = 0; i < 9; i++)
    {
        chip8_screen_draw_sprite(&chip8.screen, i + (i * 5), i, &chip8.memory.memory[(i * 5)], 5);
    }

    SDL_Init(SDL_INIT_EVERYTHING);

    // Audio setup
    SDL_AudioStream *stream = SDL_NewAudioStream(AUDIO_S16, 1, 44100, AUDIO_F32, 2, 44100);
    Sint16 samples[AUDIO_BUF_SIZE] = {0};
    if (stream == NULL)
    {
        fprintf(stderr, "Failed to create new audio stream: %s\n", SDL_GetError());
        status = 1;
        goto teardown;
    }
    // ready to use stream!
    pulse_wave(samples, AUDIO_BUF_SIZE, 512, 0.5, 0.01); // Fill samples buffer
    print_wave_table(samples, AUDIO_BUF_SIZE);
    printf("Successfully initialized audio\n");

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
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

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
                    SDL_RenderFillRect(renderer, &r);
                }
            }
        }

        SDL_RenderPresent(renderer);

        if (chip8.registers.delay_timer > 0)
        {
            SDL_Delay(100);
            chip8.registers.delay_timer--;
        }

        if (chip8.registers.sound_timer > 0)
        {
            SDL_Delay(100);
            printf("chip8.registers.sound_timer: %d\n", chip8.registers.sound_timer);

            // Fill the samples buffer with a square wave
            pulse_wave(samples, AUDIO_BUF_SIZE, 512, 0.5, 0.01);
            chip8.registers.sound_timer--;
        }
        else
        {
            // Silence, fill the samples buffer with 0s
            for (int i = 0; i < AUDIO_BUF_SIZE; i++)
            {
                samples[i] = 0;
            }
            printf("chip8.registers.sound_timer: %d\n", chip8.registers.sound_timer);
        }

        // Fill audio stream with samples buffer (pulse or silence)
        int rc = SDL_AudioStreamPut(stream, samples, AUDIO_BUF_SIZE * sizeof(Sint16));
        if (rc == -1)
        {
            fprintf(stderr, "Failed to put samples in stream: %s\n", SDL_GetError());
            goto audio_teardown;
        }
    }
audio_teardown:
    SDL_FreeAudioStream(stream);
teardown:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return status;
}

void pulse_wave(Sint16 *samples, int samples_len, Sint16 freq, double pulse_width, double vol)
{
    // Fill a Sint16 array with a pulse wave
    Sint16 const MAX = floor(65535.0 / 2.0);
    float const delta = (float)freq / SAMPLE_RATE;
    double phase = 0.00;
    Sint16 value = 0;

    // Make sure freq is below nyquist and volume isn't too loud
    // [WARNING: DO NOT USE HEADPHONES]
    assert(freq < (SAMPLE_RATE / 2) && vol > 0.00 && vol < 0.1);
    for (int i = 0; i < samples_len; i++)
    {
        if (phase < pulse_width)
            value = MAX;
        else
            value = -1 * MAX;
        Sint16 final_value = (Sint16)(value * vol);
        phase += delta;
        if (phase >= 1)
            phase -= 1;
        samples[i] = final_value;
    }
}

void print_wave_table(Sint16 *samples, int samples_len)
{
    assert(samples_len > 0);
    for (int i = 0; i < samples_len; i++)
    {
        printf("%d\t", samples[i]);
    }
    printf("\n");
}
