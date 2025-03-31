#include "../include/chip8keyboard.h"
#include <stdbool.h>
#include <assert.h>

static void chip8_keyboard_in_bounds(int key)
{
    assert(key >= 0 && key < CHIP8_TOTAL_KEYS);
}


int chip8_keyboard_map(char const *map, char real_key)
{
    int i;
    for (i = 0; i < CHIP8_TOTAL_KEYS; i++)
    {
        if (map[i] == real_key)
        {
            return i;
        }
    }
    /* Failure case */
    return -1;
}

void chip8_keyboard_up(struct chip8_keyboard *keyboard, int key)
{
    chip8_keyboard_in_bounds(key);
    keyboard->keyboard[key] = false;
}

void chip8_keyboard_down(struct chip8_keyboard *keyboard, int key)
{
    chip8_keyboard_in_bounds(key);
    keyboard->keyboard[key] = true;
}

bool chip8_keyboard_is_down(const struct chip8_keyboard *keyboard, int key)
{
    chip8_keyboard_in_bounds(key);
    return keyboard->keyboard[key];
}
