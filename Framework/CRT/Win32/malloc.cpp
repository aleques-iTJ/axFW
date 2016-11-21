#include <stdlib.h>
#include "Core/PF_MinWin.h"


void* malloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}