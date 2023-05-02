#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

// Chip8 ha una memoria di 4kb
#define CHIP8_MEMORY_SIZE 4096

// La larhezza e l'altezza dello schermo supportato
#define CHIP8_WIDTH 64
#define CHIP8_HEIGTH 32
#define EMULATOR_WINDOW_TITLE "Chip8 Window"
#define CHIP8_WINDOW_MULTIPLIER 20
#define CHIP8_PROGRAM_LOAD_ADDRESS 0x200

#define CHIP8_TOTAL_DATA_REGISTER 16
#define CHIP8_TOTAL_STACK_VALUES 16
#define CHIP8_TOTAL_KEYS 16
#define CHIP8_CHARACTER_SET_LOAD_ADDRESS 0x000
#define CHIP8_CHARACTER_SET_FINAL_ADDRESS 0x1FF
#define CHIP8_DEFAULT_SPRITE_HEIGTH 5

#endif
