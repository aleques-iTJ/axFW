#include <stdlib.h>


/*void* Memory::Allocate(size_t size, size_t alignment)
{
	return malloc(size);
}*/


void* operator new(size_t size)
{
	return malloc(size);
}


void* operator new[](size_t size)
{
	return malloc(size);
}


void operator delete(void* ptr)
{
	if (ptr != nullptr)
	{
		free(ptr);
	}
}

void operator delete(void* ptr, size_t size)
{
	if (ptr != nullptr)
	{
		free(ptr);
	}
}

void operator delete[](void* ptr) noexcept
{
	if (ptr != nullptr)
	{
		free(ptr);
	}
}

void operator delete[](void* ptr, size_t size)
{
	if (ptr != nullptr)
	{
		free(ptr);
	}
}