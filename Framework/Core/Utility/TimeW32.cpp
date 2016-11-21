#include "Core/Utility/Time.h"
#include "Core/PF_MinWin.h"


// Fix, use nested namespaces later
namespace Time
{
	namespace internal
	{
		uint64 frequency;
		uint64 current;
		uint64 previous;
	}
}


void Time::Update()
{
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	internal::previous	= internal::current;
	internal::current	= current.QuadPart;
}