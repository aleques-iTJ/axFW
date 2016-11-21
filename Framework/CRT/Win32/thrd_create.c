#include <threads.h>
#include "Core/PF_MinWin.h"


int	thrd_create(thrd_t* thread, thrd_start_t func, void* arg)
{
	thread->ptr = CreateThread(nullptr, 0, func, arg, 0, nullptr);

	return (thread->ptr != nullptr) ? thrd_success : thrd_error;
}