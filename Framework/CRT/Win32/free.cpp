#include <stdlib.h>
#include "Core/PF_MinWin.h"


void free(void* ptr)
{
	HeapFree(GetProcessHeap(), 0, ptr);
}