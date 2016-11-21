#pragma once
#pragma once
#include "Core\Types.h"
#include "Core\Containers\String.h"


typedef Delegate<void()> CvarFunc;

// This thing is a mess
struct Cvar
{
	friend class Console;


private:
	// Simple internal type identifier
	enum class CvarType
	{
		Nothing = 0,		// Unused
		Int		= 1 << 0,	// Signed int
		Float	= 1 << 1,	// Take a guess
		String	= 1 << 2,
		Func	= 1 << 3	// No argument, self contained function
	};


	// Internal value
	struct CvarValue
	{
		CvarValue () = default;
		~CvarValue() = default;


		CvarValue& operator = (bool value)
		{
			// Treat bool as an int internally
			ValueAsInt		= value;
			Type			= Cvar::CvarType::Int;
			return *this;
		}

		CvarValue& operator = (int32 value)
		{
			ValueAsInt		= value;
			Type			= CvarType::Int;
			return *this;
		}

		// Convenience
		CvarValue& operator = (uint32 value)
		{			
			ValueAsInt		= static_cast<int32>(value);
			Type			= CvarType::Int;
			return *this;
		}

		CvarValue& operator = (float32 value)
		{
			ValueAsFloat	= value;
			Type			= CvarType::Float;
			return *this;
		}

		CvarValue& operator = (char* value)
		{
			ValueAsString	= value;
			Type			= CvarType::String;
			return *this;
		}


		CvarValue& operator = (CvarFunc value)
		{
			Function		= value;
			Type			= CvarType::Func;
			return *this;
		}


		// Numeric limits on the cvar value
		const int32		ValueAsIntMinimum	= 0;
		const int32		ValueAsIntMaximum	= 2147483647;
		const float32	ValueAsFloatMinimum = 0.0f;
		const float32	ValueAsFloatMaximum = 1.0f;

		// Underlying Cvar data
		int32			ValueAsInt			= 0;
		float32			ValueAsFloat		= 0.0f;
		char*			ValueAsString		= nullptr;
		CvarFunc		Function;

		// Underlying Cvar type
		CvarType Type						= CvarType::Int;
	};



	//==============================================================================================
	// Cvar
	//==============================================================================================

public:
	template<typename T>
	Cvar(const char* name, T value)
	{
		Create(name, value);
	}


	template<typename T>
	void Create(const char* name, T value)
	{
		Name	= name;
		Value	= value;
	}


	//==============================================================================================
	// Getters
	//==============================================================================================

	// If you have access to the Cvar object, you can do a lightweight value
	// lookup instead of having to go through the Console hash table
	s32 GetInt() const
	{
		return Value.ValueAsInt;
	}

	float32 GetFloat() const
	{
		return Value.ValueAsFloat;
	}

	char* GetString() const
	{
		return Value.ValueAsString;
	}


	// Just a simple description of it
	char*		Name		= nullptr;
	char*		Description = nullptr;
	CvarValue	Value;
};