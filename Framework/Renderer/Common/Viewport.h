#pragma once
#include "Core/Math/Rect.h"

// Does what it says on the tin, can be cast to the D3D type as is
struct Viewport
{
	Rect<uint32>	Rect;

	uint32			MinDepth;
	uint32			MaxDepth;
};