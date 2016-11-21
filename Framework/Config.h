#pragma once

/*#if defined (__cplusplus)
	#define AX_API extern "C" __declspec(dllexport) 
#elif
	#define AX_API __declspec(dllexport)
#endif*/

#if defined (_WIN64)
	#define BNR_PLATFORM_WIN
	#define BNR_PLATFORM_64BIT
	#define BNR_PLATFORM_X86
	#define BNR_PLATFORM_X64
#else
	#define BNR_PLATFORM_WIN
	#define BNR_PLATFORM_32BIT
	#define BNR_PLATFORM_X86
#endif