#include "Core/System/System.h"
#include "Core/Utility/UTFConversion.h"


// Ugh
#undef GetCommandLine
#undef GetCurrentDirectory
#undef GetUserName
#undef CreateMutex
#undef WriteConsole
#undef MessageBox


String System::GetCommandLine()
{
	return String("NOT_IMPLEMENTED");
}

// Get the current directory that the exe resides in
String System::GetCurrentDirectory()
{
	wchar_t exePath		[MAX_PATH];
	wchar_t exeDirectory[MAX_PATH];
	

	// Get our own exe's path
	size_t length = GetModuleFileNameW(nullptr, exePath, MAX_PATH);

	// Walk said path backwards to the last directory
	while (exePath[length] != L'\\')
	{
		length--;
	}

	// Chop it at that point
	memcpy(exeDirectory, exePath, length);
	//exeDirectory[length + 1] = '\\';

	return String(UTF16toUTF8(exeDirectory));
}

// Gets the current computer name
String System::GetMachineName()
{
	char	str[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD	size = MAX_COMPUTERNAME_LENGTH + 1;
	GetComputerName(str, &size);

	return String(str);
}

// Gets the current user name
String System::GetUserName()
{
	char	str[64];
	// DWORD	size = 64;	
	//::GetUserNameA(str, &size);

	return String(str);
}

// Gets a friendly OS name
String System::GetOSName()
{
	return String("Windows");
}

size_t System::GetProcessorCount()
{
	SYSTEM_INFO info;
	GetNativeSystemInfo(&info);

	return info.dwNumberOfProcessors;
}


OSType System::GetOSType()
{
	return OSType::Windows;
}

//
void System::Exit(int32 code)
{
	ExitProcess(code);
}


void System::WriteConsole(StringView string)
{
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), string.GetDataPtr(), static_cast<DWORD>(string.GetLength()), nullptr, nullptr);
}

bool System::MessageBox(StringView caption, StringView string)
{
	int32 status = MessageBoxA(nullptr, string.GetDataPtr(), caption.GetDataPtr(), MB_OKCANCEL);

	if (status == 1)
		return true;

	return false;	
}

/*Mutex System::CreateMutex()
{
	return Mutex();
}*/