#if !defined(SDL_ASYNC_INCLUDED_H_)
#define SDL_ASYNC_INCLUDED_H_

/**
 * Schedule work to a thread pool.
 */
void Async_Queue_Work(void * baton);

#endif /* SDL_ASYNC_INCLUDED_H_ */