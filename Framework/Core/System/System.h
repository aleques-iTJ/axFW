#pragma once
#include "Core/Containers/String.h"
#include "Core/Containers/StringView.h"


enum class OSType
{
	Unknown,
	DOS,
	Amiga,
	Windows,
	Linux,
	Android,
	Mac
};

enum class TextColor
{
	White,
	Red,
	Green,
	Blue
};


// Provides some OS environment helpers
namespace System
{
	String	GetCommandLine		();
	String	GetCurrentDirectory	();	// Gets the current directory that the running EXE resides in
	String	GetMachineName		();
	String	GetUserName			();
	String	GetOSName			();

	size_t	GetProcessorCount	();
	 
	OSType	GetOSType			();
	void	Exit				(int32 code);

	void	WriteConsole		(StringView string);

	bool	MessageBox			(StringView caption, StringView string);
	
	//Mutex	CreateMutex			();
};