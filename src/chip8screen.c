#include "../include/chip8screen.h"
#include <assert.h>
#include <stdbool.h>

static void chip8_screen_check_bounds(int x, int y)
{
    assert(x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);
}

void chip8_screen_set(struct chip8_screen *screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    screen->pixels[y][x] = true;
}

bool chip8_screen_is_set(struct chip8_screen *screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    return screen->pixels[y][x];
}

bool chip8_screen_draw_sprite(struct chip8_screen *screen, int x, int y, char const *sprite, int num)
{
    bool pixel_collision = false;

    for (int ly = 0; ly < num; ly++) {
        char c = sprite[ly]; // gives us a row of the sprite
        // loop through the sprite layer, painting the screen offset by x & y
        for (int lx = 0; lx < 8; lx++) {
            // check if 1b >> by lx to current pos matches c (if a pixel should be there)
            if ((c & (0b10000000 >> lx)) == 0)
                continue;

            // check if there's a collision with an existing pixel
            if (screen->pixels[(ly+y) % CHIP8_HEIGHT][(lx+x) % CHIP8_WIDTH]) {
                pixel_collision = true;
            }

            // turn the pixel at pos y+ly,x+lx on or off by XORing it with whatever's there
            screen->pixels[(ly+y) % CHIP8_HEIGHT][(lx+x) % CHIP8_WIDTH] ^= true; // ^= is XOR=
        }
    }

    return pixel_collision;
}
