#include <threads.h>
#include "Core/PF_MinWin.h"


int cnd_init(cnd_t* cond)
{
	cond->ptr = cond->pf_Storage;

	if (cond)
	{
		InitializeConditionVariable(cond->ptr);
		return thrd_success;
	}

	return thrd_error;
}