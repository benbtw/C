#include "screen.h"

void init_sdl()
{
     if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_INIT error: %s", SDL_GetError());
    }
}

SDL_Window *init_window()
{
    SDL_Window *window = SDL_CreateWindow("Chip8 Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                64 * 15, 32 * 15, SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
        printf("SDL_Window error: %s", SDL_GetError());
    }
    return window;
}

SDL_Renderer *init_renderer(SDL_Window *window)
{
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == NULL)
    {
        printf("SDL_Renderer error: %s", SDL_GetError());
    }
    return renderer;
}

void screen_cleanUp(SDL_Window *window, SDL_Renderer *renderer)
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}