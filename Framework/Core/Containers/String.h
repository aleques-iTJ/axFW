#pragma once
#include "Types.h"
#include <stdlib.h>
#include <string.h>

#pragma warning(disable: 4458)


// Constants for the small size optimization
constexpr size_t BNR_SMALL_STRING_SIZE = 32;
constexpr size_t BNR_BASELINE_CAPACITY = BNR_SMALL_STRING_SIZE * sizeof(char);


// A simple String implementation
struct String
{
	constexpr static size_t NoPos = static_cast<size_t>(-1); // Invalid or undefined position


	String() = default;

	String(size_t length)
	{ 
		Reserve(length);
	}

	String(const char* string)
	{
		Assign(string);
	}

	String(const String& string)
	{
		Assign(string);
	}

	String(const String& string, size_t from, size_t to)
	{ 
		Assign(string, from, to);
	}

	String(String&& string)
	{ 
		Assign(Move(string));
	}

	~String()
	{
		Release();
	}


//==============================================================================================
// Basic operators
//==============================================================================================
	
	String& operator = (const char* string)
	{
		Assign(string);
		return *this;
	}

	String& operator = (const String& string)
	{
		Assign(string);
		return *this;
	}

	String& operator = (String&& string)
	{
		Assign(Move(string));
		return *this;
	}


	String& operator << (const char* string)
	{
		Append(string);
		return *this;
	}

	String& operator << (const String& string)
	{
		Append(string);
		return *this;
	}

	String& operator << (int32 value)
	{
		char temp[32];
		memset(temp, 0, sizeof(temp));
		itoa(value, temp, 16);
		Append(temp);
		return *this;
	}


//==============================================================================================
// Memory management related functions
//==============================================================================================

	// Allocates memory if needed, it is NOT obligated to allocate anything!
	void Reserve(size_t length)
	{
		// Either over capacity or needs to become a large string
		if (length > capacity && length > BNR_SMALL_STRING_SIZE)
		{
			const size_t newCapacity		= (capacity * 2) > length ? capacity * 2 : length;
			const size_t newCapacityInBytes	= newCapacity * sizeof(char);
			char* const	 temp				= new char[newCapacityInBytes];
			
			if (this->length > 0)
			{
				memcpy(temp, GetDataPtr(), this->length);
			}			

			// If it's already large, there is already allocated memory that must be free'd
			// otherwise, the small string is becoming large, so mark it invalid
			IsLarge() ? (void)(delete[] large) : 
						(void)(small[0] = 0);

			capacity		= newCapacity;
			capacityInBytes = newCapacityInBytes;
			large			= temp;
		}
	}

	// Invalidates the string and releases allocated memory
	void Release()
	{
		if (IsLarge())
		{
			delete[] large;
			large = nullptr;
		}

		small[0]		= 0;
		length			= 0;
		capacity		= BNR_SMALL_STRING_SIZE;
 		capacityInBytes = BNR_BASELINE_CAPACITY;
	}

	//  Invalidates the string but DOES NOT release held memory
	void Clear()
	{		
		if (IsLarge())
		{
			large[0] = 0;

			// Capacity will stay the same if there's already allocated memory
			// ...
		}

		else
		{
			small[0]		= 0;
			capacity		= BNR_SMALL_STRING_SIZE;
			capacityInBytes = BNR_BASELINE_CAPACITY;
		}

		length = 0;
	}


//==============================================================================================
// String management
//==============================================================================================

	// Copy c-string
	void Assign(const char* string)
	{
		const size_t newLength = strlen(string);

		Reserve(newLength + 1);					// Count the null character for the potential allocation
		length = newLength;
		memcpy(GetDataPtr(), string, length);

		GetDataPtr()[length] = 0;				// Length as an index will land
	}

	// Copy c-string with length
	void Assign(const char* string, size_t length)
	{
		this->length = length;

		Reserve(length + 1);
		memcpy(GetDataPtr(), string, length);
		GetDataPtr()[length] = 0;
	}

	// Copy String
	void Assign(const String& string)
	{
		length = string.length;

		if (length > 0)
		{
			Reserve(length + 1);
			memcpy(GetDataPtr(), string.GetDataPtr(), length);
			GetDataPtr()[length] = 0;
		}		
	}

	// Move String
	void Assign(String&& string)
	{
		// If there's no large string, the small string needs to be copied...
		// otherwise, this internally happened when the large string was allocated
		if (string.IsLarge() == false)
		{
			memcpy(small, string.small, string.length);
			GetDataPtr()[string.length] = 0;
		}

		large					= string.large;
		length					= string.length;
		capacity				= string.capacity;
		capacityInBytes			= string.capacityInBytes;

		string.small[0]			= 0;
		string.large			= nullptr;
		string.length			= 0;
		string.capacity			= BNR_SMALL_STRING_SIZE;
		string.capacityInBytes	= BNR_BASELINE_CAPACITY;
	}

	// Substring assignment
	void Assign(const String& string, size_t from, size_t to = NoPos)
	{
		// Consider -1 to effectively be "the end of the string"
		to == NoPos ?	length = string.length - from :
						length = to - from;

		Reserve(length + 1);
		memcpy(GetDataPtr(), string.GetDataPtr() + from, length);

		// Explicitly add the null character
		GetDataPtr()[length] = 0;
	}

	// Fill assignment
	void Assign(char character, size_t length)
	{
		Reserve(length);
		memset(GetDataPtr(), character, length);

		GetDataPtr()[length] = 0;
	}

	// Buffer assignment
	void Assign(char* string, size_t length)
	{
		Reserve(length);
		memcpy(GetDataPtr(), string, length);

		GetDataPtr()[length] = 0;
	}




	String& Append(const char* string)
	{
		const size_t apendeeLength = strlen(string);

		Reserve((apendeeLength + length) + 1);
		strcat(GetDataPtr(), string);

		length += apendeeLength;

		return *this;
	}

	String& Append(const String& string)
	{
		if (&string != this)
		{
			Append(string.GetDataPtr());
		}

		else
		{
			String result = *this;
			result.Append(string.GetDataPtr());
			*this = result;
		}

		return *this;
	}

	// Searching
	size_t Find(char character) const
	{
		size_t index = 0;

		while (index < length)
		{
			if (GetDataPtr()[index] == character)
			{
				return index;
			}

			index++;
		}

		// No match
		return NoPos;
	}

	inline String SubString(size_t from, size_t to = NoPos) const
	{
		return String(*this, from, to);
	}


//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================

	bool		IsLarge		() const { return capacity > BNR_SMALL_STRING_SIZE;										}
	bool		IsEmpty		() const { return length == 0;															}
	bool		IsValid		() const { return (capacity > BNR_SMALL_STRING_SIZE) ? large[0] == 0 : small[0] == 0;	}
	size_t		GetLength	() const { return length;																}
	size_t		GetCapacity	() const { return capacity;																}
	char*		GetDataPtr	()		 { return (capacity > BNR_SMALL_STRING_SIZE) ? large : small;					}
	const char*	GetDataPtr	() const { return (capacity > BNR_SMALL_STRING_SIZE) ? large : small;					}
	const char*	GetCString	() const { return (capacity > BNR_SMALL_STRING_SIZE) ? large : small;					}


	// This being slightly unaligned hurts me
	char&		At(size_t index)	{ return GetDataPtr()[index];		}
	const char&	At(size_t index) const	{ return GetDataPtr()[index];	}

	char&		Front()				{ return GetDataPtr()[0];			}
	const char&	Front() const		{ return GetDataPtr()[0];			}
	char&		Back ()				{ return GetDataPtr()[length - 1];	}
	const char&	Back () const		{ return GetDataPtr()[length - 1];	}


private:
	char	small[BNR_SMALL_STRING_SIZE]	= { 0 };					// Don't allocate for tiny strings, can probably get away with this in most cases
	char*	large							= nullptr;					// The large string buffer

	size_t	length							= 0;						// Number of characters in the string, NOT including the null character
	size_t	capacity						= BNR_SMALL_STRING_SIZE;	// Total capacity of the string, in characters
	size_t	capacityInBytes					= BNR_BASELINE_CAPACITY;	// Total number of bytes the string in its entirety is consuming
};