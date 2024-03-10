#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "SDL.h"

typedef struct {
    uint16_t opcode; 
    uint8_t  memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;
    uint8_t pixels[64 * 32];
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t stack[16];
    uint16_t sp;
    uint8_t key[16];	
    uint_fast8_t drawFlag;
    SDL_Texture *screen;
} chip8;

uint32_t loadRom(char* file);
void init_chip8(SDL_Renderer *renderer);
void chip8_run();
void chip8_draw(SDL_Renderer *renderer);
void chip8_keys(SDL_Event *event);

#endif