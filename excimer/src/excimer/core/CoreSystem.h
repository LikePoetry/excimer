#pragma once
#include "Core.h"

namespace Excimer
{
	namespace Internal
	{
		// Low-level System operations
		class EXCIMER_EXPORT CoreSystem
		{
		public:
			static bool Init(int argc = 0, char** argv = nullptr);
			static void Shutdown();
		};
	}
}