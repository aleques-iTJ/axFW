#pragma once
#pragma once
#include "Core/Containers/Array.h"
#include "Core/Containers/String.h"
#include "Core/Math/Math.h"
#include "Core/Math/Hash.h"

#include <intrin.h>

template <typename T>
struct Node
{
	Node(T data) :
		Data(std::move(data))
	{

	}


	bool IsFree() const
	{
		return Hash == -1;
	}

	size_t	Hash = -1;
	T		Data;
};


// An array of hashed strings - put in string, get key
template <typename T>
struct StringHashArray : private BasicStorage<Node<T>>
{
	StringHashArray(size_t count)
	{
		Reserve(count);
	}
	

	inline void Reserve(size_t count)
	{
		size_t roundedCount	= NextPowerOfTwo(count);	// Always enforce a power of 2 size, lets us avoid a modulo
		
		dirtyIndexSize		= roundedCount / sizeof(T);	// Length of the dirty index, the dirty flag is only one bit
		dirtyIndexShiftDiv	= Log2(sizeof(T));			// I honestly can't remember what the fuck this was for

		dirtyIndex.Reserve(dirtyIndexSize);
		dirtyIndex.Clear();

		// Count the required number of bits for the underlying array capacity
		// so that we can just mask the generated hash into a suitable range
		bitCount = CountReqBits(capacity);

		// Make the mask to chop the generated hashes down to the size range
		for (u32 i = 0; i < bitCount; i++)
		{			
			bitMask |= (1 << i);
		}

		BasicStorage<Node<T>>::Reserve(count);
	}

	// Inserts a value at the head end of the array
	template <typename Ref>
	inline void Insert(const char* string, Ref&& value)
	{
		// Hash it and mask it down to size for the array index
		u32 index = HashFNV(string);
		index	 &= bitMask;

		// Grow if needed...
		if (index > capacity)
		{
			Reserve(capacity + 1);
		}

		// Ignore size for now
		data[index] = Node<T>(value);
	}
	 
	inline T& Find(const char* string)
	{
		u32 index = HashFNV(string);
		index	 &= bitMask;

		return data[index].Data;
	}

//----------------------------------------------------------------

private:
	bool _isDirty(size_t index)
	{
		size_t wrappedIndex = (index + 1) & (capacity - 1);


		//dirtyIndex[index]
	}


	// Only 32 bits atm
	u32		bitCount		= 0;
	u32		bitMask			= 0;

	Array<size_t>	dirtyIndex;
	size_t		dirtyIndexSize		= 0;
	size_t		dirtyIndexShiftDiv	= 0;
};