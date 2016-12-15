#include <math.h>


#pragma function(tan)
double tan(double radians)
{
	return sin(radians) / cos(radians);
}