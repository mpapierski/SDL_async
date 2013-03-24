#include <SDL_async/SDL_async.h>
#include <assert.h>

/**
 * Async event message ID.
 * @private
 */
static int G_async_event;
/**
 * Thread pool
 */
static SDL_Thread * G_threads[10];
/**
 * Current thread pool index
 */
static int G_thread_index;

void Async_Init()
{
	int i;
	G_async_event = SDL_ASYNC_RESULT;
	for (i = 0; i < 10; i++)
	{
		G_threads[i] = 0;
	}
	G_thread_index = 0;
}

void Async_Free()
{
	int i;
	for (i = 0; i < 10; i++)
	{
		if (G_threads[i])
		{
			SDL_WaitThread(G_threads[i], NULL);
		}
	}
}

static int thread_func(void * data)
{
	async_data_t * async_data = (async_data_t*)data;
	assert(async_data && "Invalid data inside thread.");
	int result = (*(async_data->fun))(async_data->baton);

	async_data->result = result;
	SDL_Event event;

	event.type = SDL_USEREVENT;
	event.user.code = SDL_ASYNC_RESULT;
	event.user.data1 = async_data;
	event.user.data2 = 0;
	SDL_PushEvent(&event);

	return result;
}

void Async_Queue_Work(async_function_t task, void * baton)
{
	SDL_Thread * thread;
	async_data_t * task_data = (async_data_t*)malloc(sizeof(async_data_t));
	if (task_data == NULL)
	{
		// Error
		return;
	}
	task_data->result = 0;
	task_data->fun = task;
	task_data->baton = baton;
	thread = SDL_CreateThread(&thread_func, task_data);
	if (!thread)
	{
		// Error
	}
	G_threads[G_thread_index++] = thread;
}