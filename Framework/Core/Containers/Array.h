#pragma once
#include "Core/Containers/BasicStorage.h"
#include "Core/Memory/Allocator.h"


// Braindead growable managed array
template <typename T, typename Allocator = BasicAllocator<T>>
struct Array : BasicStorage<T, Allocator>
{
	using BasicStorage<T>::BasicStorage;
	using BasicStorage<T>::Reserve;
	using BasicStorage<T>::data;
	using BasicStorage<T>::size;
	using BasicStorage<T>::capacity;


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

	// Adds / pushes an element to the back of the array
	template <typename FRef>
	void PushBack(FRef&& value)
	{
		// Check if we're about to spill over the capacity with this addition to the array
		if ((size + 1) > capacity)
		{
			// Reserve will just double the capacity in this case
			Reserve(capacity + 1);
		}

		Allocator::Construct(&data[size], value);
		size++;		
	}

	// Inserts an element at a given point in the array
	template <typename FRef>
	void Insert(size_t index, FRef&& value)
	{
		if ((size + 1) > capacity)
		{
			(index > capacity) ?	Reserve(index) :		// Index may be way out there, use it instead
									Reserve(capacity + 1);	// Just double capacity otherwise
		}


		// FIX ME
		T*		insertPoint = &data[index];
		size_t	bytesToMove = (size - index) * sizeof(T);

		memmove(insertPoint + 1, insertPoint, bytesToMove);
		data[index] = Forward<FRef>(value);
	}


//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================

	T&		 At(size_t index)		{ return data[index];	}
	const T& At(size_t index) const	{ return data[index];	}
			 
	T&		 Front()				{ return data[0];		}
	const T& Front() const			{ return data[0];		}
	T&		 Back ()				{ return data[size];	}
	const T& Back () const			{ return data[size];	}

	
	// For Ranged-for
	T* begin() { return &Front();	}
	T* end	() { return &Back();	}
};