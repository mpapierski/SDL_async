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
	async_task_baton * baton = (async_task_baton*)data->baton;
	if (data->result == 0)
	{
		printf("Success writing %d to a file asynchronously...\n",
			data->result);
	}
	else
	{
		printf("Error writing data %d to a file asynchronously... Result: %d\n",
			baton->dummy,
			data->result);
	}
	free(baton);
	free(data);
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
	// The main loop is running.
	int running = 1;
	// Post some tasks
	int i, done = 0;
	for (i = 0; i < 10; i++)
	{
		async_task_baton * baton = (async_task_baton*)malloc(sizeof(async_task_baton));
		baton->dummy = 1234 + i;
		Async_Queue_Work(&async_task_function, baton);
	}

	while (running)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			puts("Quit");
			running = 0;
			break;
		case SDL_USEREVENT:
			switch (event.user.code)
			{
			case SDL_ASYNC_RESULT:
				async_task_result(event.user.data1);
				done++;
				if (done == 10)
				{
					running = 0;	
				}
				break;
			}
			break;
		}
	}
	puts("Bye.");
	return 0;
}