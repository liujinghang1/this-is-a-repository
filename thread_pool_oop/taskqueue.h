#pragma once
#include<pthread.h>
#include<queue>
#include<stdio.h>
using callback = void(*)(void*);

struct  Task
{
	Task() {
		function = nullptr;
		arg = nullptr;
	}
	Task(callback f, void* arg) {
		function = f;
		this->arg = arg;
	}
	callback function;
	void* arg;
};

class TaskQueue {

public:
	TaskQueue();
	~TaskQueue();

	void  addTask(Task& task);
	void  addTask(callback func, void* arg);

	Task takeTask();

	inline size_t taskNumber() {
		return m_queue.size();
	}

private:

	queue<Task> m_queue;
	pthread_mutex_t m_mutex;

};