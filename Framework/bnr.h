#pragma once
#include "Core/All.h"
#include "Renderer/All.h"


#if defined (BNR_PLATFORM_WIN)
	#pragma comment(lib, "kernel32")
	#pragma comment(lib, "user32")

	#if defined (BNR_PLATFORM_64BIT)
		#pragma comment(lib, "CRT-Debug-x64")
		#pragma comment(lib, "Core-Debug-x64")
		#pragma comment(lib, "Renderer-Debug-x64")
	#else
		#pragma comment(lib, "CRT-Debug-x86")
		#pragma comment(lib, "Core-Debug-x86")
	#endif
#endif