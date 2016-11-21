#pragma once
#include "Types.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <Core/Memory/Allocator.h>


// Basis for dynamic storage
template <typename T, typename Allocator = BasicAllocator<T>>
struct BasicStorage
{
	BasicStorage() = default;

	BasicStorage(size_t count)
	{
		Reserve(count);
	}

	BasicStorage(const BasicStorage& storage)
	{
		Assign(storage);
	}

	BasicStorage(BasicStorage&& storage)
	{
		Assign(Move(storage));
	}

	~BasicStorage()
	{
		Release();
	}


//==============================================================================================
// Basic operators
//==============================================================================================

	BasicStorage& operator = (const BasicStorage& storage)
	{
		Assign(storage);
		return *this;
	}

	BasicStorage& operator = (BasicStorage&& storage)
	{
		Assign(Move(storage));
		return *this;
	}


//==============================================================================================
// Memory management related functions
//==============================================================================================

	// Allocates enough memory for a given number of elements
	void Reserve(size_t count)
	{
		T* temp = nullptr;		

		if (count > capacity)
		{
			// Just double the object storage unless count is going to be even larger
			capacity = (capacity * 2) > count ? capacity * 2 : count;
			if (capacity < 8)	// Don't waste time with multiple, super tiny allocations
				capacity = 8;	// if we're just starting with no reservation

			size_t bytes = capacity * sizeof(T);
					temp = Allocator::Allocate(bytes);
		}

		if (size > 0)
		{
			Allocator::CopyConstruct(temp, data, count);				
		}

		data = temp;
	}

	// Explicitly releases allocated memory and resets all state
	void Release()
	{
		Allocator::Destruct(data, size);
		Allocator::Deallocate(data);

		data			= nullptr;
		size			= 0;
		capacity		= 0;
	}

	// Obliterate the contents of the storage
	void Clear()
	{
		Allocator::Destruct(data, size);		
		size = 0;
	}
	
	// Resets the container's size - does NOT clear allocated memory or wipe contents!
	void Reset()
	{
		size = 0;
	}


//==============================================================================================
// Buffer management
//==============================================================================================

	// Copy buffer
	void Assign(const BasicStorage& storage)
	{
		Clear();
		Reserve(storage.size);

		Allocator::CopyConstruct(data, storage.data, storage.size);

		size	 = storage.size;
		capacity = storage.capacity;
	}

	// Move buffer
	void Assign(BasicStorage&& storage)
	{
		data					= storage.data;
		size					= storage.size;
		capacity				= storage.capacity;

		storage.data			= nullptr;
		storage.size			= 0;
		storage.capacity		= 0;
	}

	void Swap(BasicStorage& storage)
	{
		// Back up
		T*		tempData	 = data;
		size_t	tempSize	 = size;
		size_t	tempCapacity = capacity;

		// ... anddd swap
		data				= storage.data;
		size				= storage.size;
		capacity			= storage.capacity;
		storage.data		= tempData;
		storage.size		= tempSize;
		storage.capacity	= tempCapacity;
	}

//==============================================================================================
// Simple helpers, getters and utility functions
//==============================================================================================

	bool	IsValid			() const	{ return data != nullptr;		}
	bool	IsEmpty			() const	{ return size == 0;				}
	bool	IsFull			() const	{ return size == capacity;		}
	size_t	GetSize			() const	{ return size;					}
	size_t	GetSizeBytes	() const	{ return size * sizeof(T);		}
	size_t	GetTypeSize		() const	{ return sizeof(T);				}
	size_t	GetTypeBits		() const	{ return CHAR_BIT * sizeof(T);	}
	size_t	GetCapacity		() const	{ return capacity;				}
	size_t	GetCapacityBytes() const	{ return capacity * sizeof(T);	}
	T*		GetDataPtr		()			{ return data;					}
	const T*GetDataPtr		() const	{ return data;					}


protected:
	T*		data			= nullptr;	// Pointer to owned data
	size_t	size			= 0;		// Current number of objects in the buffer
	size_t	capacity		= 0;		// Total capacity of the buffer, IN OBJECTS
};