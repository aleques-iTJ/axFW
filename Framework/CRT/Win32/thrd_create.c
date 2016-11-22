#include <threads.h>
#include "Core/PF_MinWin.h"


int	thrd_create(thrd_t* thread, thrd_start_t func, void* arg)
{
	thread->ptr = CreateThread(NULL, 0, func, arg, 0, NULL);

	return (thread->ptr != NULL) ? thrd_success : thrd_error;
}