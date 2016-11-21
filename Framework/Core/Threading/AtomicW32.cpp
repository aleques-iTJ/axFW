#pragma once
#include "Atomic.h"
#include <Windows.h>


int32 Atomic::Increment(int32* const variable)
{
	return InterlockedIncrementAcquire(reinterpret_cast<LONG*>(variable));
}

int32 Atomic::Decrement(int32* const variable)
{
	return InterlockedDecrementRelease(reinterpret_cast<LONG*>(variable));
}

int64 Atomic::Increment(int64* const variable)
{
	return InterlockedIncrementAcquire64(reinterpret_cast<LONGLONG*>(variable));
}

int64 Atomic::Decrement(int64* const variable)
{
	return InterlockedDecrementRelease64(reinterpret_cast<LONGLONG*>(variable));
}