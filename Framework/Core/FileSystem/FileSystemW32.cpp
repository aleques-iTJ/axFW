
#include "Core\FileSystem\FileSystem.h"
#include "Core\FileSystem\File.h"

#include "Core/PF_MinWin.h"

//==============================================================================================
// Directory management
//==============================================================================================


Array<File> FileSystem::GetFiles(StringView path)
{
	Array<File>		files;

	WIN32_FIND_DATA found;
	HANDLE			fileHandle = INVALID_HANDLE_VALUE;	


	fileHandle = FindFirstFile(path.GetDataPtr(), &found);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		// Just return the empty array
		return files;
	}

	do
	{
		File file;

		//file.filePath = found.cFileName;
		//file.fileSize = (found.nFileSizeHigh * (MAXDWORD + 1)) + found.nFileSizeLow; // WTF
		//files.PushBack(file);
	} 
	while (FindNextFile(fileHandle, &found));

	return files;
}


// Attempts to open (or create) a file for reading / writing
File FileSystem::OpenFile(StringView path, FileAccess fileAccess)
{
	// Initially do some conversion
	DWORD access		= 0;
	DWORD disposition	= 0;

	if (fileAccess == FileAccess::Read)
	{
		access		= FILE_GENERIC_READ; 
		disposition = OPEN_EXISTING;
	}

	if (fileAccess == FileAccess::Write)
	{
		access		= FILE_GENERIC_WRITE; 
		disposition = CREATE_NEW;
	}

	if (fileAccess == FileAccess::ForceCreate)
	{
		disposition = CREATE_ALWAYS;
	}

	File file;
	HANDLE handle = CreateFile(path.GetDataPtr(), access, 0, nullptr, disposition, 0, nullptr);

	if (handle != INVALID_HANDLE_VALUE)
	{
		file.handle = handle;
	}

	return file;
}



File FileSystem::OpenAndReadFile(StringView path, FileAccess fileAccess)
{
	return File();
}

size_t FileSystem::WriteFile(File& file)
{
	return 0;
	//::WriteFile(file.handle, file.data, file.
}


/*File* FileSystem::OpenFile(const String& path, FileAccess fileAccess, bool force)
{
	DWORD access;
	DWORD disposition;
	if (fileAccess == FileAccess::Read)		access		= FILE_GENERIC_READ;  disposition = OPEN_EXISTING;
	if (fileAccess == FileAccess::Write)		access		= FILE_GENERIC_WRITE; disposition = CREATE_NEW;
	if (fileAccess == FileAccess::ForceCreate)	disposition	= CREATE_ALWAYS;

};

bool FileSystem::CloseFile(File* file)
{
	return fclose(file->ptr) != 0;
}

size_t FileSystem::ReadFile(File* file)
{
	file->data = new u8[file->fileSize];
	return fread(file->data, 1, static_cast<u32>(file->fileSize), file->ptr);
}

size_t FileSystem::ReadFile(File* file, void* ptr)
{
	file->data = new u8[file->fileSize];
	return fread(ptr, 1, static_cast<u32>(file->fileSize), file->ptr);
}

size_t FileSystem::WriteFile(File* file)
{	
	return fwrite(file->data, sizeof(char), file->fileSize, file->ptr);
}*/


// Old implementation
/*File* FileSystem::OpenFile(const String& path, FileAccess fileAccess)
{
	char accessStr[4] = { 0, 0, 0, 0 };
	if	(fileAccess == FileAccess::Read)	strcpy(accessStr, "r");
	else if (fileAccess == FileAccess::Write)	strcpy(accessStr, "w");
	else if (fileAccess == FileAccess::ReadWrite)	strcpy(accessStr, "r+");
	else if (fileAccess == FileAccess::ForceCreate)	strcpy(accessStr, "w+");

	File* file	= new File;
	file->ptr	= fopen(path.GetDataPtr(), accessStr);

	if (file->ptr == nullptr)
	{
		return nullptr;
	}		

	// Grab the file size
	fseek(file->ptr, 0, SEEK_END);
	file->fileSize = ftell(file->ptr);
	fseek(file->ptr, 0, SEEK_SET);

	// Save some extra info
	file->filePath		= path;
	file->fileAccess	= fileAccess;

	return file;
};

bool FileSystem::CloseFile(File* file)
{
	return fclose(file->ptr) != 0;
}

File * FileSystem::OpenAndReadFile(const String & path, FileAccess fileAccess)
{
	return nullptr;
}

size_t FileSystem::ReadFile(File* file)
{
	file->data = new u8[file->fileSize];
	return fread(file->data, 1, static_cast<u32>(file->fileSize), file->ptr);
}

size_t FileSystem::ReadFile(File* file, void* ptr)
{
	file->data = new u8[file->fileSize];
	return fread(ptr, 1, static_cast<u32>(file->fileSize), file->ptr);
}

size_t FileSystem::WriteFile(File* file)
{	
	return fwrite(file->data, sizeof(char), file->fileSize, file->ptr);
}*/