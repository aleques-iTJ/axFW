#include <threads.h>
#include "Core/PF_MinWin.h"


thrd_t thrd_current()
{
	thrd_t temp;
	temp.ptr = GetCurrentThread();

	return temp;
}