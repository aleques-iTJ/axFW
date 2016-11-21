#pragma once
#include "Types.h"

// Unnamed struct warning
#pragma warning(disable : 4201)

struct Color
{
	// TODO actually add this stuff, but not a major point
	//static Color Add		(const Color& sourceOne, const Color& sourceTwo);
	//static Color Subtract	(const Color& sourceOne, const Color& sourceTwo);

	//static Color Negative	(const Color& source);
	//static Color Lerp		()

	union
	{
		struct
		{
			float32 Alpha;
			float32 Red;
			float32 Green;
			float32 Blue;
		};

		float32		Array[4];
		v_float128	Vector;
	};
};

struct ColorInt
{
	union
	{
		struct
		{
			uint8 Red;
			uint8 Green;
			uint8 Blue;
			uint8 Alpha;
		};

		uint8	Array[4];
		uint32	ColorARGB;
	};

	operator unsigned int()
	{
		return ColorARGB;
	}
};