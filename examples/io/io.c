#include <stdlib.h>
#include <stdio.h>

#include <SDL_async/SDL_async.h>
#include <SDL.h>

int
main()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);
	Async_Init();
	atexit(Async_Free);
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
	}
	return 0;
}