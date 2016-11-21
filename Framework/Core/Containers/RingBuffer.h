#pragma once
#include "Core/Containers/BasicStorage.h"
#include "Core/Math/Math.h"


// Simple growable ring buffer, FIFO
template <typename T, typename Allocator = BasicAllocator<T>>
struct RingBuffer : BasicStorage<T, Allocator>
{
	using BasicStorage<T>::BasicStorage;
	using BasicStorage<T>::Reserve;
	using BasicStorage<T>::data;
	using BasicStorage<T>::size;
	using BasicStorage<T>::capacity;

	RingBuffer(size_t count)
	{
		// We implicitly round up to the next power of 2 to guarantee that
		// it's possible to use a bitwise and instead of a modulo
		Reserve(NextPowerOfTwo(count));
	}	


//==============================================================================================
// Ring buffer management
//==============================================================================================

	// Inserts a value at the head end of the array
	template <typename FRef>
	void Push(FRef&& value)
	{
		// Do we need to grow?
		if ((size + 1) > capacity)
		{
			Reserve(capacity + 1);
		}

		headPos	= (tailPos + size) & (capacity - 1);
		Allocator::Construct(&data[headPos], value);

		
		// Check if we're full or wrapped around
		if (size >= capacity)
		{
			// Imagine four values on the edge of a cliff.
			// Say the oldest pushed value pushed wraps around from the tail of the ring and takes the place of the first.
			// The formerly fourth value becomes the third, the third becomes the second, and the first falls off the cliff.
			// RingBuffer works the same way.
			tailPos = _step(tailPos);
		}
		
		size++;					
	}

	// Removes a value from the tail end of the array
	inline T& Pop()
	{
		T& temp = data[tailPos];
		tailPos = _step(tailPos);

		size--;

		return temp;
	}

	// Removes a value from the front end of the array
	inline T& Steal()
	{
		T& temp = data[headPos];
		tailPos = _step(tailPos);

		size--;

		return temp;
	}


//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================

	inline bool		IsFull	() const	{ return (headPos ^ (capacity - 1)) == tailPos;			} // zero at full
	inline bool		IsEmpty	() const	{ return headPos == tailPos;							}
	inline size_t	GetSize ()			{ return (size > capacity) ? size = capacity : size;	} // WTF

private:
	// Step the ring buffer and wrap if needed
	inline size_t	_step		(size_t index) { return (index + 1) & (capacity - 1); } 
	inline size_t	_stepBack	(size_t index) { return (index - 1) & (capacity - 1); }


	size_t headPos = 0;
	size_t tailPos = 0;
};