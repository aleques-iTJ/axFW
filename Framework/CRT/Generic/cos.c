#include <math.h>


double cos(double radians)
{
	return sin(M_PI_2 - radians);
}