#include "chip8.h"
#include "screen.h"

int main(int argc, char ** argv)
{
  init_sdl();
  SDL_Window *window = init_window();
  SDL_Renderer *renderer = init_renderer(window);

  init_chip8(renderer);

  if (argc < 2)
	{
		printf("Usage: ./chip8 <rom> \n");
		return 0;
	}

  if (loadRom(argv[1]) == 0)
	{
		screen_cleanUp(window, renderer);
		return 0;
	}

  SDL_Event e;
  bool quit = false;
  while(!quit)
  {
     while (SDL_PollEvent(&e))
      {
        if (e.type == SDL_QUIT)
          quit = true;
        
        chip8_keys(&e);
      }

    chip8_run();
    chip8_draw(renderer);
  }

  screen_cleanUp(window, renderer);
  return 0;
}