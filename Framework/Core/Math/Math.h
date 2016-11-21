#pragma once
#include "Types.h"
#include <intrin.h>
#include <limits.h>

// There's currently a LOT that relies on x86!

constexpr float32 PI		= 3.141592653f;
constexpr float32 PI_RCP	= 1.0f / PI;


//==============================================================================================
// Basic assorted math and utility functions (A lot of this currently expects x86 and MSVC!)
//==============================================================================================

namespace Math
{
	// Converts degrees to radians
	inline float32 ToRadians(float32 degrees)
	{
		return degrees * (PI / 180);
	}

	// Converts radians to degrees
	inline float32 ToDegrees(float32 radians)
	{
		return radians * (180 / PI);
	}

	// Converts horizontal FOV to vertical
	inline float32 ToVertFov(float32 fov, float32 aspect)
	{
		return 0.0f;
		//return 2 * static_cast<float32>(atanf(tanf(fov * 0.5f) / aspect));
	}

	// Converts vertical FOV to horizontal
	inline float32 ToHorFov(float32 fov, float32 aspect)
	{
		return 0.0f;
		//return 2 * static_cast<float32>(atanf(tanf(fov * 0.5f) * aspect));
	}

	// Increments an integer but wrap around after a cutoff point
	inline size_t WrappedIncrement(size_t& value, size_t wrapPoint)
	{
		return ++value & (wrapPoint - 1);
	}

	// Clamp a value to a specified range
	template<typename T>
	inline T Clamp(T value, T min, T max)
	{
		return (value < min) ? min : ((value > max) ? max : value);
	}

	// Effectively a fast float to int
	int32 Round(float32 value)
	{
		return _mm_cvtss_si32(_mm_load_ss(&value));
	}

	float32 Floor(float32 value)
	{
		__m128	scalar  = _mm_load_ss(&value);
		__m128	floored = scalar;

		floored = _mm_cvtsi32_ss(floored, _mm_cvttss_si32(floored));
		scalar	= _mm_sub_ss(scalar, floored);

		return scalar.m128_f32[0];
	}

	float32 Ceil(float32 x)
	{
		return _mm_cvtss_si32(_mm_load_ss(&x));
	}

	// Clamp a float to 0.0f - 1.0f range
	inline float32 Saturate(float32 x)
	{
		return Clamp(x, 0.0f, 1.0f);
	}


	// Absolute value of an integer
	int32 Abs(int32 value)
	{
		// http://graphics.stanford.edu/~seander/bithacks.html
		const int mask = value >> (sizeof(int) * (CHAR_BIT - 1));
		return (value + mask) ^ mask;
	}

	// Abolute value of a float
	float32 Abs(float32 value)
	{
		// Just eradicate the top bit
		int32 temp = *reinterpret_cast<int32*>(&value);
		temp &= 0x7FFFFFFF;

		return static_cast<float32>(temp);
	}


	// Rounds a given integer up to the next power of 2
	size_t NextPowerOfTwo(size_t value)
	{
		// http://graphics.stanford.edu/~seander/bithacks.html

		value--;
		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;

#if defined (BNR_PLATFORM_64BIT)
		value |= value >> 32;
#endif

		return ++value;
	}
}



// Naive SinCos
/*inline void SinCos(float32 x, float32& s, float32& c)
{
#if !defined(BNR_PLATFORM_64BIT)
	__asm
	{
		mov eax, dword ptr [c]
		mov edx, dword ptr [s]
		fld	 dword ptr [x]
		fsincos
		fstp	 dword ptr [eax] // Cos first
		fstp	 dword ptr [edx] // Sin last
	}

#elif defined(BNR_PLATFORM_64BIT)
	// s = sin(x);
	// c = cos(x);
#endif
}

// Naive Cotangent
inline float32 Cot(float32 x)
{
	float32 s, c;
	SinCos(x, s, c);

	return c / s;
}*/






// CLZ, used for fast Log2
inline uint32 CountLeadingZeroes(uint32 value)
{
	uint32 index = 0;

	if (_BitScanReverse(reinterpret_cast<unsigned long*>(&index), value))
	{
		return 31 - index;
	}
	
	return 0;
}

#if defined (BNR_PLATFORM_64BIT)
inline uint64 CountLeadingZeroes(uint64 value)
{
	uint32 index = 0;

	if (_BitScanReverse64(reinterpret_cast<unsigned long*>(&index), value))
	{
		return 63 - index;
	}

	return 0;
}
#endif




// Returns the number of set (read: 1) bits in a value

inline uint32 CountSetBits(uint32 value)
{
	// http://graphics.stanford.edu/~seander/bithacks.html
	// Some kind of weird voodoo juju

	uint32 i;

	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);

	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

#if defined (BNR_PLATFORM_64BIT)
inline uint64 CountSetBits(uint64 value)
{

}
#endif

// Returns the required number of bits for a given number
inline uint32 CountReqBits(uint32 value)
{
//	DWORD count = 0;
	
//	_BitScanForward(&count, value);

//	return count;
}

#if defined (BNR_PLATFORM_64BIT)
inline uint64 CountReqBits(uint64 value)
{	
//	DWORD count = 0;

	// What kind of name is DWORD64?
//	_BitScanForward64(&count, value);

//	return count;
}
#endif



// ROL
template<typename T>
inline T RotateLeft(T value)
{
	const size_t typeSize = (sizeof(T) * CHAR_BIT) - 1;

	return (value << 1) | (value >> typeSize);
}

inline uint32 Log2(uint32 value)
{
	return CountLeadingZeroes(value);
}

#if defined (BNR_PLATFORM_64BIT)
inline uint64 Log2(uint64 value)
{
	return CountLeadingZeroes(value);
}
#endif

