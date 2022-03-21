#include "thread_pool.h"



#include<string.h>
#include<iostream>
#include<unistd.h>
using namespace std;

ThreadPool::ThreadPool(int min, int max)
{
	m_taskQ = new TaskQueue;
	do {
		m_minNum = min;
		m_maxNum = max;
		m_busyNum = 0;
		m_aliveNum = min;


		m_threadIDs = new pthread_t[max];
		if (m_threadIDs = nullptr) {
			cout << "malloc thrad_t[] fail..." << endl;
			break;
		}

		memset(m_threadIDs, 0, sizeof(pthread_t) * max);

		if (pthread_mutex_init(&m_lock, NULL) != 0 ||
			pthread_cond_init(&m_notempty, NULL) != 0) {
			cout << "init mutex or condition fail..." << endl;
			break;
		}

		for (int i = 0; i < m_minNum; ++i) {
			pthread_create(&m_threadIDs[i], NULL, worker, this);
			cout << "create son thread,id:" << to_string(m_threadIDs[i]) << endl;
		}

		pthread_create(&m_managerID, NULL, manager, this);

	} while (0);
}
ThreadPool::~ThreadPool()
{
	m_shutdown = 1;

	pthread_join(m_managerID, NULL);

	for (int i = 0; i < m_aliveNum; ++i) {
		pthread_cond_signal(&m_notempty);
	}

	if (m_taskQ) delete m_taskQ;
	if (m_threadIDs) delete[] m_threadIDs;
	pthread_mutex_destroy(&m_lock);
	pthread_cond_destroy(&m_notempty);
}

void ThreadPool::addTask(Task task)
{
	if (m_shutdown) {
		return;
	}
	m_taskQ->addTask(task);
	pthread_cond_signal(&m_notempty);
}

int ThreadPool::getBusyNumber()
{
	int busyNum = 0;
	pthread_mutex_lock(&m_lock);
	busyNum = m_busyNum;
	pthread_mutex_unlock(&m_lock);
	return busyNum;

}

int ThreadPool::getAliveNumber()
{
	int threadNum = 0;

	pthread_mutex_lock(&m_lock);
	threadNum = m_aliveNum;
	pthread_mutex_unlock(&m_lock);
	return threadNum;
}


void* ThreadPool::worker(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (true) {

		pthread_mutex_lock(&pool->m_lock);
		while (pool->m_taskQ->taskNumber() == 0 && !pool->m_shutdown) {
			cout << "thread" << to_string(pthread_self()) << "waiting..." << endl;
			pthread_cond_wait(&pool->m_notempty, &pool->m_lock);

			if (pool->m_exitNum > 0) {
				pool->m_exitNum--;
				if (pool->m_aliveNum > pool->m_minNum) {
					pool->m_aliveNum--;
					pthread_mutex_unlock(&pool->m_lock);
					pool->threadExit();
				}
			}
		}

		if (pool->m_shutdown) {
			pthread_mutex_unlock(&pool->m_lock);
			pool->threadExit();
		}

		Task task = pool->m_taskQ->takeTask();
		pool->m_busyNum++;
		pthread_mutex_unlock(&pool->m_lock);

		cout << "thread" << to_string(pthread_self()) << "start_working" << endl;
		task.function(task.arg);
		delete task.arg;
		task.arg = nullptr;

		cout << "thread" << to_string(pthread_self()) << "end working" << endl;
		pthread_mutex_lock(&pool->m_lock);
		pool->m_busyNum--;
		pthread_mutex_unlock(&pool->m_lock);
	}
	return nullptr;
}

void* ThreadPool::manager(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (!pool->m_shutdown) {

		sleep(3);
		pthread_mutex_lock(&pool->m_lock);
		int queueSize = pool->m_taskQ->taskNumber();
		int liveNum = pool->m_aliveNum;
		int busyNum = pool->m_busyNum;
		pthread_mutex_unlock(&pool->m_lock);

		const int NUMBER = 2;

		if (queueSize > liveNum && liveNum < pool->m_maxNum) {

			pthread_mutex_lock(&pool->m_lock);
			int num = 0;
			for (int i = 0; i < pool->m_maxNum && num < NUMBER && pool->m_aliveNum < pool->m_maxNum; ++i) {

				if (pool->m_threadIDs[i] == 0) {
					pthread_create(&pool->m_threadIDs[i], NULL, worker, pool);
					num++;
					pool->m_aliveNum++;
				}
			}
			pthread_mutex_unlock(&pool->m_lock);
		}

		if (busyNum * 2 < liveNum && liveNum > pool->m_minNum) {
			pthread_mutex_lock(&pool->m_lock);
			pool->m_exitNum = NUMBER;
			pthread_mutex_unlock(&pool->m_lock);
			for (int i = 0; i < NUMBER; ++i) {
				pthread_cond_signal(&pool->m_notempty);
			}
		}
	}
	return nullptr;
}

void ThreadPool::threadExit()
{
	pthread_t tid = pthread_self();
	for (int i = 0; i < m_maxNum; ++i) {
		if (m_threadIDs[i] == tid) {
			cout << "threadEXIT function " << to_string(pthread_self()) << "exiting" << endl;
			m_threadIDs[i] = 0;
			break;
		}
	}
	pthread_exit(NULL);
}














