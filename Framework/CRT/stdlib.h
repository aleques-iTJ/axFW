#pragma once
#include <stdint.h>
#include "API.inl"


// Memory manipulation
AX_API void*	malloc	(size_t size);
AX_API void*	realloc (void* ptr, size_t size);
AX_API void		free	(void* ptr);


// Conversion
AX_API int		atoi	 (const char* str);
AX_API int		atoi_fast(const char* str);
AX_API double	atof	 (const char* str);

AX_API char*	itoa	 (int value, char* str, int base); // Non standard