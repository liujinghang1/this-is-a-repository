#pragma once

#include"taskqueue.h"



#include<pthread.h>
class ThreadPool {

public:

	ThreadPool(int min, int max);
	~ThreadPool();

	void addTask(Task task);

	int getBusyNumber();

	int getAliveNumber();

private:
	static void* worker(void* arg);
	static void* manager(void* arg);
	void threadExit();

private:
	pthread_mutex_t m_lock;
	pthread_cond_t m_notempty;
	pthread_t m_managerID;
	pthread_t* m_threadIDs;
	TaskQueue* m_taskQ;
	int m_minNum;
	int m_maxNum;
	int m_busyNum;
	int m_aliveNum;
	int m_exitNum;
	bool m_shutdown = false;

};