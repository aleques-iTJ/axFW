#include <stdint.h>
#include <emmintrin.h>


// Vectorized SSE2 memcpy
void* memcpy(void* __restrict dest, const void* __restrict src, size_t count)
{
	// This seems pretty decent performance wise
	const int8_t* const	source		= (int8_t*)src;
	int8_t* const		destination = (int8_t*)dest;

	size_t i = 0;

	// Start with 64 byte copies, make sure we don't overrun
	for (; i + 64 < count; i += 64)
	{
		// Not sure why MSVC decides to only use the XMM0 register when copying more than 64 bytes
		// Anyway, performance doesn't really seem better anyway copying bigger blocks???

		// [i + (n << 4)] indexes on 16 byte spans - so multiply by 16
		const __m128i xmm0 = _mm_load_si128(( __m128i*)&source[i + 0 ]);
		const __m128i xmm1 = _mm_load_si128(( __m128i*)&source[i + 16]);
		const __m128i xmm2 = _mm_load_si128(( __m128i*)&source[i + 32]);
		const __m128i xmm3 = _mm_load_si128(( __m128i*)&source[i + 48]);

		// Writes, non temporal
		_mm_stream_si128((__m128i* const)&destination[i + 0 ], xmm0);
		_mm_stream_si128((__m128i* const)&destination[i + 16], xmm1);
		_mm_stream_si128((__m128i* const)&destination[i + 32], xmm2);
		_mm_stream_si128((__m128i* const)&destination[i + 48], xmm3);
	}

	for (; i + 16 < count; i += 16)
	{
		// dqword about it
		const __m128i dqword =	_mm_load_si128	((const __m128i* const)&source[i]);
								_mm_stream_si128((__m128i* const)&destination[i], dqword);
	}

	for (; i + 4 < count; i += 4)
	{
		*(uint32_t* const)&destination[i] = *(const uint32_t* const)&source[i];
	}

	for (; i < count; i += 1)
	{
		*(int8_t* const)&destination[i] = *(const int8_t* const)&source[i];
	}

	// Need to fence because of the non temporal moves
	_mm_sfence();

	return destination;
}