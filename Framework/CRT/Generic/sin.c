#include <math.h>


#pragma function(sin)
double sin(double radians)
{
	const double c1 =  1.00000000000000000000e+00;
	const double c2 = -1.66666671633720397949e-01;
	const double c3 =  8.33333376795053482056e-03;
	const double c4 = -1.98412497411482036114e-04;
	const double c5 =  2.75565571428160183132e-06;
	const double c6 = -2.50368472620721149724e-08;
	const double c7 =  1.58849267073435385100e-10;
	const double c8 = -6.58925550841432672300e-13;

	const int	mod = (int)(radians * (1.0 / M_PI));
	const double x1	= (radians - (double)(mod * M_PI));
	const double x2	= x1 * x1;
	const double cx = (mod & 1) ? -x1 : x1;

	double accum;
	accum = x2 * c8;
	accum = x2 * (c7 + accum);
	accum = x2 * (c6 + accum);
	accum = x2 * (c5 + accum);
	accum = x2 * (c4 + accum);
	accum = x2 * (c3 + accum);
	accum = x2 * (c2 + accum);
	accum = cx * (c1 + accum);

	return accum;
}