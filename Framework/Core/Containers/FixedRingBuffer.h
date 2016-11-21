#pragma once
#include "Types.h"


// Simple and fast fixed size ring buffer, REQUIRES power of 2 capacity
template <typename T, size_t capacity>
struct FixedRingBuffer
{

//==============================================================================================
// Ring buffer management
//==============================================================================================

	// Adds an object at the head end of the ring buffer
	template <typename FRef>
	void Push(FRef&& value)
	{
		data[headPos] = Forward<FRef>(value);

		if (IsFull())
		{
			// If it's full, the first value put in is dropped
			tailPos = Step(tailPos);
		}

		headPos = Step(headPos);
	}

	// Removes an object from the tail end of the array
	T& Pop()
	{
		T& temp	= data[tailPos];
		tailPos	= Step(tailPos);

		return temp;
	}


//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================

	// Simple helpers
	bool		IsFull	() const		{ return (headPos ^ (capacity - 1)) == tailPos;			} // Zero at full
	bool		IsEmpty	() const		{ return headPos == tailPos;							}
	//size_t	GetSize	()				{ return (size > capacity) ? size = capacity : size;	}

private:
	// Helper to walk along the ring
	inline size_t	Step	(size_t index)	{ return (index + 1) & (capacity - 1);					} 


private:
	T		data[capacity];
	size_t	headPos			= 0;
	size_t	tailPos			= 0;
};