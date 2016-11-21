#include <threads.h>
#include "Core/PF_MinWin.h"


int	mtx_lock(mtx_t* mutex)
{
	EnterCriticalSection(mutex->ptr);
	return thrd_success;
}