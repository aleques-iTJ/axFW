#pragma once
#include <stdint.h>
#include "API.inl"


AX_API size_t	strlen (const char* str);
AX_API char*	strcpy (char* dest, const char* src);
AX_API char*	strcat (char* dest, const char* src);
AX_API
AX_API void*	memcpy (void* dest, const void* src, size_t count);
AX_API void*	memmove(void* dest, const void* src, size_t count);
AX_API void*	memset (void* dest, int value, size_t count);
AX_API int		memcmp (const void* ptr1, const void* ptr2, size_t count);


// Some extensions
AX_API void*	memcpy_reverse(void* dest, const void* src, size_t count);