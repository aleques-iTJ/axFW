#include <stdlib.h>
#include <stdbool.h>


// Converts an ASCII character to a float
// TODO: This currently doesn't parse exponent, hex, inf, nan!
double atof(const char* str)
{
	size_t left			= 0;		// Input on the left of the decimal point
	size_t right		= 0;		// Input on the right of the decimal point

	bool   isFractional	= false;	// Set after encountering a decimal point
	size_t numToShift	= 1;		// Used as the divisor for adding the right / fractional portion
	

	for (size_t i = 0; str[i] != '\0'; i++)
	{
		// Make sure it's actually a number...
		if (str[i] >= '0' && str[i] <= '9')
		{
			if (!isFractional)
			{
				// Can start building the left hand side of 
				// the input since we've yet to hit a decimal

				left		= (left * 10)  + (str[i] - '0');		
				numToShift *= 10;							
			}

			else
				right		= (right * 10) + (str[i] - '0');
		}

		// Only other valid input possibility is if we hit a decimal point
		else if (str[i] == '.')
		{
			// In which case, flag it that we're on the other 
			// side of the decimal point and skip the iteration
			isFractional = true;
			continue;
		}
	}

	double value = (double)left + (double)(right / numToShift);

	// aaaand sign if needed
	if (str[0] == '-')
	{
		value = -value;
	}

	return value;
}