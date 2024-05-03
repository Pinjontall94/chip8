#ifndef CHIP8REGISTERS
#define CHIP8REGISTERS

#include "config.h"
struct chip8_registers
{
    // Define V0-VF as a char array (each char is 8b, so this works)
    unsigned char V[CHIP8_NUM_DATA_REGISTERS];

    // Make the memory address 16b register
    unsigned short I;

    // Make the 8b timer registers
    unsigned char delay_timer;
    unsigned char sound_timer;

    // Make the 16b program counter and the 8b stack pointer
    unsigned short PC;
    unsigned char SP;
};

#endif // CHIP8REGISTERS
