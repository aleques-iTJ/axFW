#pragma once
#include "Core/FileSystem/FileSystem.h"


// Simple file wrapper
struct File
{
	friend File		FileSystem::OpenFile (StringView, FileAccess);
	friend size_t	FileSystem::WriteFile(File&);


	File() = default;
	~File()
	{
		delete data;
	}


	//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================

	inline size_t		GetSize		()		 { return fileSize; }
	inline String		GetPath		()		 { return filePath; }
	inline const void*	GetDataPtr	() const { return data; }
	inline void*		GetDataPtr	()		 { return data; }


protected:
	void*		handle		= nullptr;
	void*		data		= nullptr;

	size_t		fileSize	= 0;
	String		filePath	= "C:\\";

	bool		isLoaded	= false;
};