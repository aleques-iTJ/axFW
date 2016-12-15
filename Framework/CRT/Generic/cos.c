#include <math.h>


#pragma function(cos)
double cos(double radians)
{
	return sin(M_PI_2 - radians);
}