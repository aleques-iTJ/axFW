#include "Core/Containers/Array.h"

// This should probably be rethought a bit so it's more friendly for other platforms
struct VideoMode
{
	VideoMode();
	VideoMode(int32 width, int32 height, int32 refresh = 0, bool fullscreen = false, bool vsync = false);

	//----------------------------------------------------------------

	static Array<VideoMode>		GetAvailableModes();
	static VideoMode			GetCurrentMode();

private:
	int32	Width		= 0;
	int32	Height		= 0;
	int32	Refresh		= 0;
	bool	Fullscreen	= false;
	bool	VSync		= false;
};