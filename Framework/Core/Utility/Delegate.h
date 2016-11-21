#pragma once


template <typename T>
class Delegate;


// Member functions
template <typename Class, typename ReturnType, typename... Params>
class Delegate<ReturnType(Class::*)(Params...)>
{
public:
	typedef ReturnType(Class::*FuncPtr)(Params...);

	Delegate(FuncPtr func, Class& instance) :
		instance(instance),
		func	(func)
	{

	}

	ReturnType operator()(Params... args) const
	{
		return (instance.*func)(args...);
	}

private:
	Class&	instance;
	FuncPtr func;
};


// Free functions
template <typename RetValue, typename... Params>
class Delegate<RetValue(*)(Params...)>
{
public:
	typedef RetValue(*FuncPtr)(Params...);


	Delegate(FuncPtr function) :
		func(function)
	{

	}

	RetValue operator()(Params... args) const
	{
		return (*func)(args...);
	}

private:
	FuncPtr func;
};


// Helpers
template <typename FuncPtr, typename Class>
Delegate<FuncPtr> Bind(FuncPtr function, const Class& instance)
{
	return Delegate<FuncPtr>(function, instance);
}

template <typename FuncPtr>
Delegate<FuncPtr> Bind(FuncPtr function)
{
	return Delegate<FuncPtr>(function);
}