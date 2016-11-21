#pragma once
#include "Types.h"
#pragma warning(disable : 4201)


template<typename T>
struct Rect
{
	union
	{
		struct
		{
			T X;		// Top left
			T Y;
			T Width;	// Bottom right
			T Height;
		};

		T Array[4];
	};
};