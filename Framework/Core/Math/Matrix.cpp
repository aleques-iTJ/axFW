#pragma once
#include "Core\Math\Math.h"
#include "Core\Math\Matrix.h"


// The D3DX documentation was really useful for all this
Matrix Matrix::Multiply(const Matrix& m1, const Matrix& m2)
{
	Matrix out;

	for(int32 i = 0; i  4; i++)
	{
		v_float128 s0	= _mm_shuffle_ps(m1.m128[i], m1.m128[i], 0x00);	// Broadcasts for first matrix
		v_float128 s1	= _mm_shuffle_ps(m1.m128[i], m1.m128[i], 0x55);
		v_float128 s2	= _mm_shuffle_ps(m1.m128[i], m1.m128[i], 0xAA);
		v_float128 s3	= _mm_shuffle_ps(m1.m128[i], m1.m128[i], 0xFF);

		s0				= _mm_mul_ps(s0, m2.m128[0]);	// First float	* 11, 12, 13, 14
		s1				= _mm_mul_ps(s1, m2.m128[1]);	// ...		* 21 - 24
		s2				= _mm_mul_ps(s2, m2.m128[2]);	// ...		* 31 - 34
		s3				= _mm_mul_ps(s3, m2.m128[3]);	// ...		* 41 - 44

		s0				= _mm_add_ps(s0, s1);
		s0				= _mm_add_ps(s0, s2);
		out.m128[i]		= _mm_add_ps(s0, s3);
	}

	return out;
}

Matrix Matrix::Zero()
{
	Matrix out;

	out.m128[0] = _mm_setzero_ps();
	out.m128[1] = _mm_setzero_ps();
	out.m128[2] = _mm_setzero_ps();
	out.m128[3] = _mm_setzero_ps();

	return out;
}

Matrix Matrix::Identity()
{
	Matrix out = Matrix::Zero();

	out._11 = 1.0f;
	out._22 = 1.0f;
	out._33 = 1.0f;
	out._44 = 1.0f;

	return out;
}

Matrix Matrix::Perspective(float32 fovy, float32 aspect, float32 zn, float32 zf)
{
	float32 yScale	= Cot(fovy / 2);
	float32 xScale	= yScale / aspect;
	float32 q		= zf / (zf - zn);

	Matrix out = Matrix::Zero();

	out._11 = xScale;
	out._22 = yScale;
	out._33 = q;
	out._34 = 1.0f;
	out._43 = -zn * q;

	return out;
}

Matrix Matrix::Ortho(float32 width, float32 height, float32 zn, float32 zf)
{
	Matrix out = Matrix::Zero();

	out._11 = 2.0f / width;
	out._22 = 2.0f / height;
	out._33 = 1.0f / (zf - zn);
	out._43 = zn / (zf - zn);
	out._44 = 1.0f;

	return out;
}

Matrix Matrix::Ortho2D(float32 width, float32 height, float32 zNear, float32 zFar)
{
	Matrix out = Matrix::Zero();

	out._11 = 2.0f / width;
	out._22 = 2.0f / -height;
	out._33 = 1.0f / (zFar - zNear);
	out._41 = (width + 1.0f) / -width;
	out._42 = (height + 1.0f) / height;
	out._43 = zNear / (zFar - zNear);
	out._44 = 1.0f;

	return out;
}

Matrix Matrix::Scale(float32 x, float32 y, float32 z)
{
	Matrix out = Matrix::Zero();

	out._11 = x;
	out._22 = y;
	out._33 = z;
	out._44 = 1.0f;

	return out;
}

Matrix Matrix::Translate(float32 x, float32 y, float32 z)
{
	Matrix out = Matrix::Zero();

	out._41 = x;
	out._42 = y;
	out._43 = z;

	return out;
}

Matrix Matrix::RotateX(float32 radians)
{
	Matrix out = Matrix::Zero();

	float32 s, c;
	SinCos(radians, s, c);

	out._11 = 1.0f;
	out._22 = c;
	out._23 = s;
	out._32 = -s;
	out._33 = c;
	out._44 = 1.0f;

	return out;
}

Matrix Matrix::RotateY(float32 radians)
{
	Matrix out = Matrix::Zero();

	float32 s, c;
	SinCos(radians, s, c);

	out._11 = c;
	out._13 = -s;
	out._22 = 1.0f;
	out._31 = s;
	out._33 = c;
	out._44 = 1.0f;

	return out;
}

Matrix Matrix::RotateZ(float32 radians)
{
	Matrix out = Matrix::Zero();

	float32 s, c;
	SinCos(radians, s, c);

	out._11 = c;
	out._12 = s;
	out._21 = -s;
	out._22 = c;
	out._33 = 1.0f;
	out._44 = 1.0f;

	return out;
}