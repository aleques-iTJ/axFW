#pragma once

namespace MemoryAlloc
{
	//==============================================================================================
	// Internal helpers for memory alignment / tracking
	//==============================================================================================

	namespace internal
	{
		// Used to prefix the allocated memory block with information
		struct AllocationTag
		{
			void*	pointer;	// The non-biased original pointer
			size_t	size;		// The size of the allocation
		};


		// Total needed memory to fit the tag and meet the size requirements after considering the alignment
		inline size_t GetTotalNeededMem(size_t size, size_t alignment)
		{
			// Just return the total size needed and let the native OS allocator align (and round up) to whatever it wants
			// We'll bias the pointer later after it's actually been allocated

			return (size + sizeof(AllocationTag)) + (alignment - 1);
		}


		// Returns an aligned address, MUST be power of 2!
		inline void* GetAlignedAddr(void* addr, size_t alignment)
		{
			// Takes an address instead of a raw size
			// return reinterpret_cast<void*>((reinterpret_cast<size_t>(addr) + (alignment - 1)) & ~(alignment - 1));

			// Using 16 byte alignment as an example, the last 4 bits would be 0'd
			// 16 dec = 0001 0000 bin ... but we need the rest of the bits to stay

			// Bitwise AND will only keep 1's...		
			//  16 dec =             0001 0000
			//  15 dec = (repeating) 0000 1111 - need to opposite, so just flip (bitwise complement)
			// -16 dec = (repeating) 1111 0000
			// ~(alignment - 1);

			// return reinterpret_cast<void*>((reinterpret_cast<size_t>(addr) + (alignment - 1)) & ~(alignment - 1));

			// Sweet jesus
			return reinterpret_cast<void*>(((reinterpret_cast<size_t>(addr) + sizeof(AllocationTag)) + (alignment - 1)) & ~(alignment - 1));
		}
	}


	//==============================================================================================
	// Memory allocation (these force 16 byte alignment!)
	//==============================================================================================

	// WARNING: THESE ARE STILL BORKED ON 32 BIT

	void*	Allocate	(size_t size, size_t alignment = 16);
	void*	Reallocate	(size_t size, void* ptr, size_t alignment = 16);
	void	Free		(void* ptr);
}