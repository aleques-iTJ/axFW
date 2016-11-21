#pragma once
#include "Core/System/System.h"


struct Mutex
{
public:
	Mutex ();
	~Mutex();

	void Lock	();
	void Unlock	();


private:
	alignas (16) int8 pf_Storage[40];
};