#include "../include/chip8stack.h"
#include "../include/chip8.h"
#include <assert.h>

static void chip8_stack_in_bounds(struct chip8 *chip8)
{
    // make sure that 0 <= SP <= 15
    // SP is signed, so no need to check for negative
    assert(chip8->registers.SP < CHIP8_TOTAL_STACK_DEPTH);
}

void chip8_stack_push(struct chip8 *chip8, unsigned short val)
{
    chip8_stack_in_bounds(chip8);
    chip8->stack.stack[chip8->registers.SP] = val;
    chip8->registers.SP++;
}

unsigned short chip8_stack_pop(struct chip8 *chip8)
{
    chip8->registers.SP--;
    chip8_stack_in_bounds(chip8);
    unsigned short val = chip8->stack.stack[chip8->registers.SP];
    return val;
}
