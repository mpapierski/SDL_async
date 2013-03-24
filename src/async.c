#include <SDL_async/SDL_async.h>
#include <assert.h>

/**
 * Async event message ID.
 * @private
 */
static int G_async_event;
/**
 * Thread pool list.
 */
static SDL_Thread ** G_threads;
/**
 * Current thread pool count.
 */
static int G_thread_count;
/**
 * Task queue
 */
static async_queue_t * G_queue;
/**
 * Each thread waits for this condition forever.
 */
SDL_cond * G_queue_cond;
/**
 * Queue guard.
 */
SDL_mutex * G_queue_guard;
/**
 * Global state for all threads.
 */
int G_is_running;

/**
 * Put a job in the queue.
 * @private
 */
static void Async_Queue_Put(async_data_t * data)
{
	int result = SDL_LockMutex(G_queue_guard);
	if (result < 0)
	{
		fprintf(stderr, "Error acquiring mutex.\n");
		return;
	}
	async_queue_t * head = G_queue;
	async_queue_t * next = (async_queue_t*)malloc(sizeof(async_queue_t));
	if (!next)
	{
		puts("Error adding new queue item");
		// Error
		return;
	}
	next->next = head;
	next->data = data;
	G_queue = next;
	SDL_CondSignal(G_queue_cond);
	SDL_UnlockMutex(G_queue_guard);
}

static int thread_func(void * data)
{
	// There is at least one way to do it better.
	int alive = 1;
	int result;
	while (G_is_running && alive)
	{
		SDL_LockMutex(G_queue_guard);
		while (!G_queue)
		{
			if (!G_is_running)
			{
				alive = 0;
				break;
			}
			if (SDL_CondWait(G_queue_cond, G_queue_guard) < 0)
			{
				fprintf(stderr, "Error waiting for a task... (%s)\n", SDL_GetError());
				break;
			}
		}
		if (!alive)
		{
			SDL_UnlockMutex(G_queue_guard);
			break;
		}
		assert(G_queue && "The queue is empty!");
		async_queue_t * head = G_queue;
		G_queue = G_queue->next;
		SDL_UnlockMutex(G_queue_guard);
		// Run the task (we are out of critical section)
		int exitcode = head->data->fun(head->data->baton);
		head->data->result = exitcode;
		// Notify event loop
		SDL_Event event;
		event.type = SDL_USEREVENT;
		event.user.code = G_async_event;
		event.user.data1 = head->data;
		event.user.data2 = 0;
		free(head);
		SDL_PushEvent(&event);
	}
	return 0;
}

void Async_Init(int size)
{
	G_is_running = 1;
	int i;
	G_async_event = SDL_ASYNC_RESULT;
	// Create conditional variable
	G_queue_cond = SDL_CreateCond();
	G_queue_guard = SDL_CreateMutex();
	// Empty queue
	G_queue = 0;
	// Create threads
	G_thread_count = size;
	G_threads = (SDL_Thread**)malloc(size * sizeof(SDL_Thread*));
	for (i = 0; i < size; i++)
	{
		G_threads[i] = SDL_CreateThread(&thread_func, 0);
	}
}

void Async_Free()
{
	int i;
	// Stop all threads
	G_is_running = 0;
	// Notify all threads
	SDL_CondBroadcast(G_queue_cond);
	// Wait for threads
	for (i = 0; i < G_thread_count; i++)
	{
		if (G_threads[i])
		{
			SDL_WaitThread(G_threads[i], NULL);
		}
	}
	free(G_threads);
	SDL_DestroyCond(G_queue_cond);
	SDL_DestroyMutex(G_queue_guard);
}

void Async_Queue_Work(async_function_t task, void * baton)
{
	async_data_t * task_data = (async_data_t*)malloc(sizeof(async_data_t));
	task_data->result = 0;
	task_data->fun = task;
	task_data->baton = baton;
	Async_Queue_Put(task_data);
}