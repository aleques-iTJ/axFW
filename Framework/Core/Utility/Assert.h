#include "Core/Containers/String.h"
#include "Core/System/System.h"


namespace Debug
{
	namespace internal
	{
		void Assert(bool condition, LWString file, LWString function, LWString message)
		{
			/*if (!condition)
			{
				String error;
				error.Append(file.GetDataPtr());
				error.Append("\n");
				error.Append(function.GetDataPtr());
				error.Append("\n");
				error.Append(message.GetDataPtr());
				System::MessageBox("Uh oh...", error);
				System::Exit(3);
			}*/
		}
	}


}

#define Assert(condition, string) Debug::internal::Assert(condition, __FILE__, __FUNCTION__, string);