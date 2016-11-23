#include <threads.h>
#include "Core/PF_MinWin.h"


int	cnd_signal(cnd_t* cond)
{
	WakeConditionVariable(cond->ptr);
	return thrd_success;
}