#include <threads.h>
#include "Core/PF_MinWin.h"


int cnd_wait(cnd_t* cond, mtx_t* mutex)
{
	BOOL ret = SleepConditionVariableCS(cond->ptr, mutex->ptr, INFINITE);

	return (ret > 0) ? thrd_success : thrd_error;
}