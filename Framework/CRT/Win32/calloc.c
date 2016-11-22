#include <stdlib.h>
#include "Core/PF_MinWin.h"


void* calloc(size_t size)
{
	if (size == 0)
	{
		// HeapAlloc() doesn't seem to return a null pointer if size is 0
		return NULL;
	}

	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}