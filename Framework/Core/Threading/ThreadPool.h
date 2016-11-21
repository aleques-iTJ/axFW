#pragma once
#include "Core/Containers/Array.h"
#include "Core/Utility/Delegate.h"
#include <threads.h>

using TaskFunc = thrd_start_t;// Delegate<bool(*)(void* data)>;

struct Task
{
	Task*		Parent;
	TaskFunc	Function;	
	int32		TasksLeft;	// Atomic

	int8		padding[40];
};


class ThreadPool
{
	void Create	(size_t numberOfThreads);
	void Destroy();


	void Consume();

	static void WorkerFunc(ThreadPool* const threadPool);

private:
	Array<thrd_t> threads;
};