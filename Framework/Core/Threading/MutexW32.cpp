#pragma once
#include "Core/Threading/Mutex.h"
#include <Core/PF_MinWin.h>

#include <new>


//==============================================================================================
// PLATFORM SPECIFIC -- Win32 internals
//==============================================================================================

namespace
{
	// Native underlying platform data
	struct MutexPF
	{
		// Native platform handles
		CRITICAL_SECTION mutex;
	};

	MutexPF* pf = nullptr;
}


//==============================================================================================
// Mutex setup and destruction
//==============================================================================================

Mutex::Mutex()
{
	InitPFStorage(MutexPF, pf_Storage);
}


Mutex::~Mutex()
{
	DeleteCriticalSection(&pf->mutex);
}



void Mutex::Lock()
{
	EnterCriticalSection(&pf->mutex);
}

void Mutex::Unlock()
{
	LeaveCriticalSection(&pf->mutex);
}
