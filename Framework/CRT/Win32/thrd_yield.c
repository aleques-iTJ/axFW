#include <threads.h>
#include "Core/PF_MinWin.h"


void thrd_yield()
{
	YieldProcessor();
	Sleep(0);
}