#pragma once
#include "Types.h"


#pragma warning(disable : 4201)

struct Matrix;
struct Vector
{
	static Vector Multiply(const Vector& vector, const Matrix& matrix);


	align16 union
	{
		struct
		{
			float32 x;
			float32 y;
			float32 z;
			float32 w;
		};

		float32		v[4];
		v_float128	v128;
	};
};