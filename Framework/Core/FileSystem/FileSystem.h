#pragma once
#include "Core/Containers/Array.h"
#include "Core/Containers/String.h"
#include "Core/Containers/StringView.h"
#pragma warning(disable : 4251)


enum class FileAccess : unsigned int
{
	// Without the Create flag, it will only operate on existing files
	Read		= 0,
	Write		= 1 << 1,
	ReadWrite	= 1 << 2,
	ForceCreate	= 1 << 3
};

struct File;

namespace FileSystem
{

//==============================================================================================
// Directory management
//==============================================================================================

	//Array<File>		GetFiles		(StringView path);
	//Array<String>	GetDirectories	(StringView path);


//==============================================================================================
// File IO
//==============================================================================================


	File	OpenFile			(StringView path, FileAccess fileAccess);
	File	OpenAndReadFile		(StringView path, FileAccess fileAccess);
	bool	CloseFile			(File& file);

	size_t	ReadFile			(File& file);
	size_t	WriteFile			(File& file);

	bool	MoveFile			(StringView source, StringView destination);
	bool	CopyFile			(StringView source, StringView destination);
};