#include <stdlib.h>


// Converts an integer to an ASCII string
char* itoa(int value, char* str, int base)
{
	char	buf[32];
	size_t	i = 0;

	do
	{
		buf[i++] = (value % 10) + '0';
	} while ((value /= 10) > 0);


	size_t num = i;
	str[num] = 0;

	for (size_t j = 0; j < num; i--, j++)
	{
		str[j] = buf[i - 1];
	}

	return str;
}