#include "ThreadPool.h"
#include "Core/System/System.h"
#include "Core/Containers/RingBuffer.h"
#include "Core/PF_MinWin.h"


void ThreadPool::Create(size_t numberOfThreads /* = 0 */)
{
	if (numberOfThreads == 0 || numberOfThreads == -1)
	{
		numberOfThreads = System::GetProcessorCount();
	}
	

	for (size_t i = 0; i < numberOfThreads; i++)
	{
		thrd_t worker;
		int status = thrd_create(&worker, (thrd_start_t)WorkerFunc, this);

		if (status == thrd_success)
		{
			threads.PushBack(worker);
		}
	}
}

void ThreadPool::Destroy()
{
	// FUCK
}


void ThreadPool::Consume()
{
	Task	currentTask;
	mtx_t	mutex;

	RingBuffer<Task> taskList;

	while (true)
	{
		mtx_lock(&mutex);


		while (taskQueue.GetSize() == 0)
		{
			condition.Wait(&mutex);
		}

		currentTask = taskQueue.Pop();
		mtx_unlock(&mutex);

		currentTask.Function(currentTask.);
	}
}

void ThreadPool::WorkerFunc(ThreadPool* const threadPool)
{

}

RingBuffer<Task>* GetWorkerThreadTaskQueue()
{

}