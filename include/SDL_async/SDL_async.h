#if !defined(SDL_ASYNC_INCLUDED_H_)
#define SDL_ASYNC_INCLUDED_H_

#include <stdlib.h>
#include <SDL.h>

/**
 * Default ASYNC user event.
 */
#define SDL_ASYNC_RESULT 100

/**
 * Task function type.
 */
typedef int (*async_function_t)(void * /* baton */);

/**
 * Things associated with a single task.
 */
typedef struct
{
	// Task exit code.
	int result;
	// Function to execute inside a thread.
	async_function_t fun;
	// Additional data associated with task.
	void * baton;
} async_data_t;

/**
 * Task queue
 */
typedef struct async_queue_t
{
	// Current element.
	async_data_t * data;
	// Next task in queue.
	struct async_queue_t * next;
} async_queue_t;

/**
 * Initialize a thread pool. At the beggining the threading pool size is 0,
 * and later the more tasks are submitted the pool grows to the specified
 * limit.
 *
 * Every thread in a pool starts in a "WAITING" state, but then when a task
 * comes it begins processing.
 * @param size Pool size.
 */
void Async_Init(int size);

/**
 * Stop all threads, wait for currently processing tasks and join threads.
 */
void Async_Free();

/**
 * Schedule work to a thread pool.
 * When a job is submitted to the thread pool, it waits for execution there,
 * and later when the job is done the result is sent back as an SDL event.
 * @param task The function is called from the thread with a `baton`
 * parameter.
 * @param baton Additional data asociated with the task.
 */
void Async_Queue_Work(async_function_t task, void * baton);

#endif /* SDL_ASYNC_INCLUDED_H_ */