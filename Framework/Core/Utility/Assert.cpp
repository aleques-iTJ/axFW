#pragma once
#include "Assert.h"
#include "Core/Containers/String.h"
#include "Core/System/System.h"


void Debug::internal::_Assert(bool condition, const char* const file, const char* const function, const char* const message)
{
	if (condition == false)
	{
			String error;
			error << file		<< "\n";
			error << function	<< "\n";
			error << message	<< "\n";

			System::MessageBox("Uh oh...", error);
			System::Exit(-1);
	}
}