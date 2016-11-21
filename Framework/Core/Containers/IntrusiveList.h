#pragma once
#include "Types.h"


// Intrusive linked list - nodes are their own entities

// INCOMPLETE
template<typename T>
class IntrusiveList
{
public:
	struct Node
	{

	private:
		T		data;
		Node*	next;
	};

//----------------------------------------------------------------

	IntrusiveList() :
		root	(nullptr),
		size	(0),
		capacity(0)
	{

	}

	~IntrusiveList()
	{

	}

//----------------------------------------------------------------

	inline void PushBack(const T& value)
	{
		// Check if we're about to spill over the capacity with this addition to the array
		if ((size + 1) > capacity)
		{
			// Reserve will just double the capacity
			Reserve(capacity + 1);
		}

		root[size].data = value;
		//root[size].
		size++;
	}

	void Link(Node* node, Node* nextNode)
	{

	}

	void Unlink(Node* node)
	{

	}
	
	
//----------------------------------------------------------------

private:
	Node* root;

	size_t	size;		// Number of elements in the list
	size_t	capacity;	// How many nodes we have allocated
	size_t	capacityInBytes;
};