#pragma once
#include "Core/Containers/String.h"


// NOTE: So far only Clang seems to actually be able to deduce this straight to a constant...
size_t constexpr bnr_strlen_ce(const char* const str)
{
	return (*str == 0) ? 0 : 1 + bnr_strlen_ce(str + 1);
}

struct  StringView;
using	LWString = StringView;


// Non-owning reference to a string
struct StringView
{
	constexpr StringView() :
		string(nullptr),
		length(0)
	{

	}

	constexpr StringView(const char* const string) :
		string(string),
		length(bnr_strlen_ce(string))
	{

	}

	constexpr StringView(const char* const string, size_t size) :
		string(string),
		length(size)
	{

	}

	StringView(const StringView& string) = default;

	StringView(const String& string) :
		string(string.GetDataPtr()),
		length(string.GetLength())
	{

	}

//==============================================================================================
// Basic operators
//==============================================================================================
	
	StringView& operator = (const StringView& view) = default;
	StringView& operator = (const String& string)
	{
		this->string = string.GetDataPtr();
		this->length = string.GetLength();

		return *this;
	};

//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================


	constexpr bool			IsEmpty		() const { return length == 0;						}
	constexpr bool			IsValid		() const { return string == nullptr ? true : false;	}
												 
	constexpr size_t		GetLength	() const { return length; }
	constexpr const char*	GetDataPtr	() const { return string; }
	
	// UHGG the alignment
	constexpr const char&	Front		() const { return GetDataPtr()[0];			}
	constexpr const char&	Back		() const { return GetDataPtr()[length];		}
	constexpr const char&	At(size_t index) const { return GetDataPtr()[index];	}


private:
	const char*	string = nullptr;
	size_t 		length = 0;	
};