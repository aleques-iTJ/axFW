#include <string.h>


// Generic strcat()
char* strcat(char* dest, const char* src)
{
	const size_t startingPoint = strlen(dest);
	return strcpy(&dest[startingPoint], src);
}