#include <threads.h>
#include "Core/PF_MinWin.h"


int mtx_init(mtx_t* mutex, int type)
{
	mutex->ptr = mutex->pf_Storage;

	if (mutex)
	{
		InitializeCriticalSection(mutex->ptr);
		return thrd_success;
	}

	return thrd_error;
}