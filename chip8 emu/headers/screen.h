#ifndef SCREEN_H
#define SCREEN_H
#include <SDL.h>
#include <stdio.h>

SDL_Window *init_window();
SDL_Renderer *init_renderer();
void init_sdl();
void screen_cleanUp(SDL_Window *window, SDL_Renderer *renderer);

#endif