#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <SDL_async/SDL_async.h>
#include <SDL.h>

typedef struct
{
	int dummy;
} async_task_baton;

static int async_task_function(void * data)
{
	async_task_baton * baton = (async_task_baton*)data;
	FILE * tmp = tmpfile();
	if (!tmp)
	{
		puts("error");
		return 1;
	}
	fprintf(tmp, "Value: %d\n", baton->dummy);
	fclose(tmp);
	return 0;
}

static void async_task_result(async_data_t * data)
{
	if (data->result == 0)
	{
		printf("Success writing %d to a file asynchronously...\n",
			data->result);
	}
	else
	{
		async_task_baton * baton = (async_task_baton*)data->baton;
		printf("Error writing data %d to a file asynchronously... Result: %d\n",
			baton->dummy,
			data->result);
	}
}

int
main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);
	Async_Init();
	atexit(Async_Free);
	int running = 1;
	SDL_Event event;
	
	// Post some tasks
	async_task_baton * baton = (async_task_baton*)malloc(sizeof(async_task_baton));
	baton->dummy = 1234;
	Async_Queue_Work(&async_task_function, baton);
	while (running)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			puts("Quit");
			break;
		case SDL_USEREVENT:
			switch (event.user.code)
			{
			case SDL_ASYNC_RESULT:
				async_task_result(event.user.data1);
				running = 0;
				break;
			}
			break;
		}
	}
	puts("Bye.");
	free(baton);
	return 0;
}