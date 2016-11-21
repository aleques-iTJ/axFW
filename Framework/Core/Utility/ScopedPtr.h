#pragma once
#include "Types.h"

template <typename T>
class ScopedPtr
{
public:
	ScopedPtr();

	ScopedPtr(T* ptr) :
		pointer(ptr)
	{

	}

	ScopedPtr(ScopedPtr&& ptr) :
		pointer(x.pointer)
	{
		x.pointer = nullptr;
	}

	// Copy is disabled, move instead
	ScopedPtr(const ScopedPtr&) = delete;


private:
	T* pointer = nullptr;
};