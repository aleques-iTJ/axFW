#pragma once

template <typename T1, typename T2>
struct Pair
{
	Pair() = default;

	Pair(const T1& first, const T2& second) :
		First	(first),
		Second	(second)
	{

	}

	Pair(T1&& first, T2&& second) :
		First (std::forward(first)),
		Second(std::forward(second))
	{
		
	}

	Pair& operator = (const Pair& pair)
	{
		First	= pair.First;
		Second	= pair.Second;

		return *this;
	}

	Pair& operator = (Pair&& pair)
	{
		//First	= std::move(pair.First);
		//Second  = std::move(pair.Second);

		return *this;
	}

	~Pair()
	{

	}

//----------------------------------------------------------------

	T1 First;
	T2 Second;
};