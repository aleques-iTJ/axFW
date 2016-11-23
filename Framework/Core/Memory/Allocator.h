#pragma once
#include <stdlib.h>
#include <new>


namespace internal
{
	template<typename T, bool isPOD> struct Construct;
	template<typename T, bool isPOD> struct CopyConstruct;
	template<typename T, bool isPOD> struct Destruct;
}

template<typename T>
struct internal::Construct<T, true>
{
	static void Func(T* dest, size_t count)
	{
		// Nothing, it's a POD type
	}
};

template<typename T>
struct internal::Construct<T, false>
{
	static void Func(T* dest, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			new(&dest[i]) T;
		}
	}
};


template<typename T>
struct internal::CopyConstruct<T, true>
{
	static void Func(T* dest, const T* src, size_t count)
	{
		// Nothing, it's a POD type
	}
};

template<typename T>
struct internal::CopyConstruct<T, false>
{
	static void Func(T* dest, const T* src, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			new(&dest[i]) T(src[i]);
		}
	}
};



template<typename T>
struct internal::Destruct<T, true>
{
	static void Func(T* ptr, size_t count)
	{
		// Nothing, it's a POD type
	}
};

template<typename T>
struct internal::Destruct<T, false>
{
	static void Func(T* ptr, size_t count)
	{
		for (size_t i = 0; i < count; i++)
		{
			ptr[i].~T();
		}
	}
};


template<typename T, bool isPOD = __is_pod(T)>
struct BasicAllocator
{
	static T* Allocate(size_t size)
	{
		return reinterpret_cast<T*>(malloc(size));
	}

	static T* Reallocate(T* ptr, size_t size)
	{
		return reinterpret_cast<T*>(realloc(ptr, size));
	}

	static void Deallocate(T* ptr)
	{
		return free(ptr);
	}

	//////////////////////////////////////////////////////////////////////////

	static void Construct(T* dest, T&& arg)
	{
		new(static_cast<void*>(dest)) T(Forward<T>(arg));
	}

	static void Construct(T* dest, const T* src, size_t count)
	{
		internal::CopyConstruct<T, isPOD>::Func(dest, src, count);
	}

	static void CopyConstruct(T* dest, const T* src, size_t count)
	{
		internal::CopyConstruct<T, isPOD>::Func(dest, src, count);
	}

	static void Destruct(T* ptr, size_t count)
	{
		internal::Destruct<T, isPOD>::Func(ptr, count);
	}
};