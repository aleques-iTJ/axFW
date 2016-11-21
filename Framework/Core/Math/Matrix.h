#pragma once
#include "Types.h"

#pragma warning(disable : 4201)

struct Matrix
{
	static Matrix Multiply		(const Matrix& m1, const Matrix& m2);	

	static Matrix Zero			();
	static Matrix Identity		();

	static Matrix Perspective	(float32 fovY, float32 aspectRatio, float32 zNear, float32 zFar);
	static Matrix Ortho			(float32 width, float32 height, float32 zNear, float32 zFar);
	static Matrix Ortho2D		(float32 width, float32 height, float32 zNear, float32 zFar);
	//static inline Matrix& LookAt	(const Vector* eye, const Vector* at, const Vector* up);

	static Matrix Scale			(float32 x, float32 y, float32 z);
	static Matrix Translate		(float32 x, float32 y, float32 z);
	static Matrix RotateX		(float32 radians);
	static Matrix RotateY		(float32 radians);
	static Matrix RotateZ		(float32 radians);


//==============================================================================================
// Streaming operations - they operate on batches rather than a singular object
//==============================================================================================

	// static Matrix __vectorcall StreamMultiply(u32 num, const Matrix& m1, const Matrix& m2);


	align16 union
	{
		struct
		{
			float32	_11, _12, _13, _14,
					_21, _22, _23, _24,
					_31, _32, _33, _34,
					_41, _42, _43, _44;
		};

		float32		m[4][4];
		v_float128	m128[4];
	};
};