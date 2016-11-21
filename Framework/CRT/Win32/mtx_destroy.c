#include <threads.h>
#include "Core/PF_MinWin.h"


void mtx_destroy(mtx_t* mutex)
{
	DeleteCriticalSection(mutex->ptr);
}