#pragma once
#include "Core/Containers/String.h"


namespace Hash
{
	// The FNV hashes are from
	// http://www.isthe.com/chongo/tech/comp/fnv/

	uint32 FNV_32(const char* string);
	uint32 FNV_32(const String& string);

	uint64 FNV_64(const char* string);
	uint64 FNV_64(const String& string);
}