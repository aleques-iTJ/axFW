#include <threads.h>
#include "Core/PF_MinWin.h"


int	mtx_unlock(mtx_t* mutex)
{
	LeaveCriticalSection(mutex->ptr);
	return thrd_success;
}