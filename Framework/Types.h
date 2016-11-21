#pragma once
#include "Config.h"


//==============================================================================================
// Our own types and some name prettifying
//==============================================================================================

#include <stdint.h>
using int8		= int8_t;
using int16		= int16_t;
using int32		= int32_t;
using int64		= int64_t;
using uint8		= uint8_t;
using uint16	= uint16_t;
using uint32	= uint32_t;
using uint64	= uint64_t;
using float32	= float;
using float64	= double;


//==============================================================================================
// Macros / helpers
//==============================================================================================

#define InitPFStorage(StorageType, PFStorage)\
static_assert(__is_pod(StorageType) == true, "Internal platform data must be POD.");\
static_assert(sizeof(StorageType) <= sizeof(PFStorage), "Platform data storage is too small.");\
pf = reinterpret_cast<StorageType*>(PFStorage);

#define CheckImplStorage(StorageType, PFStorage)


#define align16			__declspec(align(16))
#define align32			__declspec(align(32))
#define lengthof(x)		(sizeof((x)) / sizeof((x)[0]))

#define enum_flags(T)\
enum class T;\
inline T	operator | (T lhs, T rhs) { return static_cast<T>(static_cast<intptr_t>(lhs) | static_cast<intptr_t>(rhs));	}\
inline bool	operator & (T lhs, T rhs) { return (static_cast<intptr_t>(lhs) & static_cast<intptr_t>(rhs)) != 0;			}


// Replacements for std::move / std::forward
template <typename T> struct RemoveReference								{ typedef T Type; };
template <typename T> struct RemoveReference<T&>							{ typedef T Type; };
template <typename T> struct RemoveReference<T&&>							{ typedef T Type; };
template <typename T> T&& Forward(typename RemoveReference<T>::Type& arg)	{ return static_cast<T&&>(arg); }
template <typename T> T&& Forward(typename RemoveReference<T>::Type&& arg)	{ return static_cast<T&&>(arg); }
template <typename T> typename RemoveReference<T>::Type&& Move(T&& arg)		{ return static_cast<typename RemoveReference<T>::Type&&>(arg); }


//==============================================================================================
// Platform specific
//==============================================================================================

#if defined (BNR_PLATFORM_WIN)

	#ifdef __midl
		#define _VCRT_ALIGN(x)
	#else
		#define _VCRT_ALIGN(x) __declspec(align(x))
	#endif


#define _SIZE_T_DEFINED
#define _PTRDIFF_T_DEFINED
#define _INTPTR_T_DEFINED
#define _UINTPTR_T_DEFINED

	#include <xmmintrin.h>
	#include <emmintrin.h>
	using v_float128	= __m128;
	using v_int128		= __m128i;

	/*#define WIN32_LEAN_AND_MEAN

	#define _CRT_SECURE_NO_WARNINGS 1
	#define _HAS_EXCEPTIONS 1

	#define _INC_MALLOC // Because otherwise xmmintrin wants malloc.h
	#undef _WIN32
void* malloc	(size_t size);
void* realloc	(size_t size, void* ptr);
void  free		(void* ptr);
	#include <xmmintrin.h>
	#include <emmintrin.h>
	using v_float128	= __m128;
	using v_int128		= __m128i;
	

	// Hackery to try and please the UCRT headers and whatnot even though we don't use them
	#define __midl 1
	#undef	_X86_

	using byte		= unsigned char;
	using errno_t	= int;
	
	int			_wcsicmp(const wchar_t* string1, const wchar_t* string2);
	size_t		wcslen	(const wchar_t* str);
	wchar_t*	wcscpy	(wchar_t* dst, const wchar_t* src);
	errno_t		wcscpy_s(wchar_t* strDestination, size_t numberOfElements, const wchar_t* strSource);
	wchar_t*	wcsrchr	(const wchar_t* str, wchar_t c);
	wchar_t*	wcschr	(const wchar_t* str, wchar_t c);*/
#endif