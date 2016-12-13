#include <stdint.h>
#include <emmintrin.h>


// Memset
void* memset(void* dest, int value, size_t count)
{
	const __m128i	val128	= _mm_set1_epi8((char)value);
	const int32_t	val32	= (value << 24) |
							  (value << 16) |
							  (value << 8 ) |
							   value;
	const int8_t	val8	= (int8_t)value;
	int8_t* const	ptr		= (int8_t*)dest;

	size_t i = 0;

	// Start with 64 byte copies, make sure we don't overrun
	for (; i + 64 < count; i += 64)
	{
		// Writes, non temporal
		_mm_stream_si128((__m128i* const)&ptr[i + 0 ], val128);
		_mm_stream_si128((__m128i* const)&ptr[i + 16], val128);
		_mm_stream_si128((__m128i* const)&ptr[i + 32], val128);
		_mm_stream_si128((__m128i* const)&ptr[i + 48], val128);
	}

	for (; i + 16 < count; i += 16)
	{
		_mm_stream_si128((__m128i* const)&ptr[i], val128);
	}

	for (; i + 4 < count; i += 4)
	{
		*(uint32_t* const)&ptr[i] = val32;
	}

	for (; i < count; i += 1)
	{
		*(int8_t* const)&ptr[i] = val8;
	}

	return ptr;
}
