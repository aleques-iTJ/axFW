#include <stdlib.h>


// Converts an ASCII character to an integer
int atoi(const char* str)
{
	int32_t value = 0;

	// Walk the string...
	for (size_t i = 0; str[i] != '\0'; i++)
	{
		// Only convert sane input
		if (str[i] >= '0' && str[i] <= '9')
		{
			// 0 as a string character is numerically 48 dec
			// So, to go from the ASCII character to decimal, subtract 48

			// The multiply by 10 effectively shifts the existing number over
			// Since value is initially 0, the multiplication does nothing on the first round

			value = (value * 10) + (str[i] - '0');
		}

		else
			break;
	}

	// Finally, negate if needed...
	if (str[0] == '-')
	{
		value = -value;
	}

	return value;
}


// Skips sanity and negative checks, use only if you can guarantee the input meets these requirements!
int atoi_fast(const char* str)
{
	int32_t value = 0;

	for (size_t i = 0; str[i] != '\0'; i++)
	{
		value = (value * 10) + (str[i] - '0');
	}

	return value;
}