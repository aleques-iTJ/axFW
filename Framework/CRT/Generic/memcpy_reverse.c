#include <stdlib.h>


// TODO: Faster implementation
void* memcpy_reverse(void* dest, const void* src, size_t count)
{
	const int8_t* const	source		= (int8_t*)src;
	int8_t* const		destination = (int8_t*)dest;

	
	// Just a straight bytewise copy from the back
	for (size_t i = (count - 1); i > 0; i--)
	{
		destination[i] = source[i];
	}

	return destination;
}