#include <threads.h>
#include "Core/PF_MinWin.h"


int	cnd_broadcast(cnd_t* cond)
{
	WakeAllConditionVariable(cond->ptr);
	return thrd_success;
}