#pragma once

#pragma once
#ifndef _THREADPOOL_H
#define _THREADPOOL_H

typedef struct ThreadPool ThreadPool;

// create thread_pool initialized
ThreadPool* threadPoolCreate(int min, int max, int queueSize);

//destroy thread_pool
int threadPoolDestroy(ThreadPool* pool);

//add task to thread_pool
void threadPoolAdd(ThreadPool* pool, void(*func)(void*), void* arg);

//get the number of working thread from thread_pool
int threadPoolBusyNum(ThreadPool* pool);

//get the number of thread alive from thread_pool
int threadPoolAliveNum(ThreadPool* pool);


//task func
void* worker(void* arg);

//mamger func
void* manager(void* arg);

//one thread exit
void threadExit(ThreadPool* pool);
#endif
