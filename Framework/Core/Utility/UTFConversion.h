#pragma once
#include "Core/Containers/String.h"
#include "Core/PF_MinWin.h"



// TODO: Fix this
inline String UTF16toUTF8(const wchar_t* const utf16)
{
	char	small[128];
	String	utf8;
	LPCWCH	wide = reinterpret_cast<LPCWCH>(utf16);

	int length = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);

	if (length > 1)
	{
		WideCharToMultiByte(CP_UTF8, 0, wide, -1, small, length, nullptr, nullptr);
	}

	return String(small);
}