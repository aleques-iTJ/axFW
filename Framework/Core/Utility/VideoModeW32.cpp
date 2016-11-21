#include "VideoMode.h"
#include <Windows.h>


VideoMode::VideoMode()
{

}

VideoMode::VideoMode(int32 width, int32 height, int32 refresh, bool fullscreen, bool vsync)
{

}


// Grabs a list of possible video modes
Array<VideoMode> VideoMode::GetAvailableModes()
{
	DEVMODE				mode;
	size_t				count = 0;
	Array<VideoMode>	modes;

	mode.dmSize = sizeof(DEVMODE);
	while (EnumDisplaySettings(0, count, &mode))
	{
		modes.PushBack(VideoMode(mode.dmPelsWidth,
								 mode.dmPelsHeight,
								 mode.dmDisplayFrequency,
								 false,
								 false));
		count++;
	}

	return modes;
}

// Gets the current video mode
VideoMode VideoMode::GetCurrentMode()
{
	DEVMODE		mode;
	VideoMode	modes;

	mode.dmSize = sizeof(DEVMODE);
	EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &mode);

	modes.Width			= mode.dmPelsWidth;
	modes.Height		= mode.dmPelsHeight;
	modes.Refresh		= mode.dmDisplayFrequency;
	modes.Fullscreen	= false;
	modes.VSync			= false;

	return modes;
}