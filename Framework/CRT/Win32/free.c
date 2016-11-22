#include <stdlib.h>
#include "Core/PF_MinWin.h"


void free(void* ptr)
{
	if (ptr != NULL)
	{
		HeapFree(GetProcessHeap(), 0, ptr);
	}	
}