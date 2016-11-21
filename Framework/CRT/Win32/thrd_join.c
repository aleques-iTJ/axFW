#include <threads.h>
#include "Core/PF_MinWin.h"


int	thrd_join(thrd_t thread, int* res)
{
	WaitForSingleObject(thread.ptr, INFINITE);
	return 0;
}