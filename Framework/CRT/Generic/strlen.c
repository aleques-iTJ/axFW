#include <string.h>


// Generic strlen()
#pragma function(strlen)
size_t strlen(const char* str)
{
	size_t length = 0;

 	while (*str++ != '\0')
	{
		length++;
	}

	return length;
}