#include <string.h>


// Generic strchr()
char* strchr(char* str, int character)
{
	while (*str++ != '\0')
	{
		if (*str == character)
		{
			return str;
		}
	}

	return NULL;
}