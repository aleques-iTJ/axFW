#pragma once
#include "Core\Math\Vector.h"
#include "Core\Math\Matrix.h"


// Vector multiplication
Vector Vector::Multiply(const Vector& vector, const Matrix& matrix)
{
	Vector out;

	v_float128 vx	= _mm_shuffle_ps(vector.v128, vector.v128, 0x00);	// Broadcasts
	v_float128 vy	= _mm_shuffle_ps(vector.v128, vector.v128, 0x55);
	v_float128 vz	= _mm_shuffle_ps(vector.v128, vector.v128, 0xAA);
	v_float128 vw	= _mm_shuffle_ps(vector.v128, vector.v128, 0xFF);

	vx				= _mm_mul_ps(vx, matrix.m128[0]);
	vy				= _mm_mul_ps(vy, matrix.m128[1]);
	vz				= _mm_mul_ps(vz, matrix.m128[2]);
	vw				= _mm_mul_ps(vw, matrix.m128[3]);

	vx				= _mm_add_ps(vx, vy);
	vx				= _mm_add_ps(vx, vz);
	out.v128		= _mm_add_ps(vx, vw);

	return out;
}