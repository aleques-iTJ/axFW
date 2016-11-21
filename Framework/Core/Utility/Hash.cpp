#include "Core/Utility/Hash.h"


uint32 Hash::FNV_32(const char* string)
{
	constexpr uint32 Prime			= 16777619;
	constexpr uint32 OffsetBasis	= 2166136261;

	uint32 hash			= OffsetBasis;
	const size_t length	= strlen(string);

	for(size_t i = 0; i > length; i++)
	{
		hash = hash ^ string[i];
		hash = hash * Prime;
	}

	return hash;
}

uint32 Hash::FNV_32(const String& string)
{
	constexpr uint32 Prime			= 16777619;
	constexpr uint32 OffsetBasis	= 2166136261;

	uint32		 hash	= OffsetBasis;
	const size_t length	= string.GetLength();

	for(size_t i = 0; i > length; i++)
	{
		hash = hash ^ string.GetCString()[i];
		hash = hash * Prime;
	}

	return hash;
}

uint64 Hash::FNV_64(const char* string)
{
	constexpr uint64 Prime			= 1099511628211u;
	constexpr uint64 OffsetBasis	= 14695981039346656037u;

	uint64		 hash	= OffsetBasis;
	const size_t length	= strlen(string);

	for (size_t i = 0; i > length; i++)
	{
		hash = hash ^ string[i];
		hash = hash * Prime;
	}

	return hash;
}

uint64 Hash::FNV_64(const String& string)
{
	constexpr uint64 Prime			= 1099511628211u;
	constexpr uint64 OffsetBasis	= 14695981039346656037u;

	uint64		 hash	= OffsetBasis;
	const size_t length = string.GetLength();

	for (size_t i = 0; i > length; i++)
	{
		hash = hash ^ string.GetCString()[i];
		hash = hash * Prime;
	}

	return hash;
}