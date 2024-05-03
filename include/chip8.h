#ifndef CHIP8_H
#define CHIP8_H

#include "chip8memory.h"
#include "chip8registers.h"
#include "config.h"
struct chip8
{
    struct chip8_memory memory;
    struct chip8_registers registers;
};

#endif
