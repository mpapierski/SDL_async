#if !defined(SDL_ASYNC_INCLUDED_H_)
#define SDL_ASYNC_INCLUDED_H_

/**
 * Task function type.
 */
typedef void (*async_function_t)(void * /* baton */);

/**
 * Initialize a thread pool. At the beggining the threading pool size is 0,
 * and later the more tasks are submitted the pool grows to the specified
 * limit.
 *
 * Every thread in a pool starts in a "WAITING" state, but then when a task
 * comes it begins processing.
 */
void Async_Init();

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