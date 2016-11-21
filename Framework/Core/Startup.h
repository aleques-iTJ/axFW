#pragma once

#include "Core/PF_MinWin.h"

#include "Utility\Time.h"
#include "FileSystem\FileSystem.h"


int	 __cdecl main();
void __cdecl mainBNRStartup()
{
	// Init some globals while we're here

	//const File StdOut = FileSystem::OpenFile("CONOUT$");
	

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	Time::internal::frequency	= freq.QuadPart;
	Time::internal::current		= 0;
	Time::internal::previous	= 0;

	//Allocator*	g_Allocator		= static_cast<Allocator*>(bnr_malloc(sizeof(Allocator)));
	//FileSystem*	g_FileSystem	= new FileSystem;
	//ThreadPool*	g_ThreadPool	= new ThreadPool;

	ExitProcess(main());
}

extern "C" int			_fltused = 1;
extern "C" int __cdecl	_purecall(void) { return 0; }