#pragma once
#include "Types.h"


// Simple fixed size array
template <typename T, size_t capacity>
struct FixedArray
{

//==============================================================================================
// Basic operators
//==============================================================================================

	T& operator[](size_t index)
	{
		return data[index];
	}

	const T& operator[](size_t index) const
	{
		return data[index];
	}


//==============================================================================================
// Array management
//==============================================================================================

	// Adds an item to the array
	template <typename Ref>
	void PushBack(Ref&& value)
	{
		data[size] = Forward<Ref>(value);
		size++;
	}

	// Inserts an item into the array, moving other elements to fit it
	template <typename Ref>
	void Insert(size_t index, Ref&& value)
	{
		// FIX ME
		data[index] = Forward<Ref>(value);
	}

	// Completely zeros the container
	void Clear()
	{
		size = 0;
		memset(data, 0, capacity * sizeof(T));
	}

	// Reset the size, does not actually clear the contents
	void Reset()
	{
		size = 0;
	}


//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================

	inline T&		At			(size_t index)			{ return data[index];	}
	inline const T& At			(size_t index) const	{ return data[index];	}

	inline T&		Front		()						{ return data[0];		}
	inline const T& Front		() const				{ return data[0];		}
	inline T&		Back		()						{ return data[size];	}
	inline const T& Back		() const				{ return data[size];	}

	inline bool		IsEmpty		() const				{ return size == 0;		}
	inline size_t	GetSize		() const				{ return size;			}
	inline size_t	GetCapacity	() const				{ return capacity;		}
	inline T* const GetDataPtr	() const				{ return data;			}
	inline T*		GetDataPtr	()						{ return data;			}

	
	// For Ranged-for
	T* begin() { return &Front();	}
	T* end	() { return &Back();	}

protected:
	T		data[capacity];
	size_t	size = 0;
};