#pragma once
#include "../Types.h"
#include "Core/Containers/Array.h"


using DeviceID = uint16;

class Renderer
{
public:
	static Array<DeviceID> EnumerateDevices();


protected:
//	Viewport  viewport;
//	FrameInfo frameInfo;

	int32 r_width;
	int32 r_height;
	int32 r_refresh;
	int32 r_fullscreen;
	int32 r_vsync;
	int32 r_maxfps;
};