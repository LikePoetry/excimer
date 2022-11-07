#pragma once
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"

namespace Excimer
{
	namespace Graphics
	{
		class CommandBuffer
		{
		public:
			virtual ~CommandBuffer() = default;

			static CommandBuffer* Create();

			virtual bool Init(bool primary = true) = 0;
			virtual bool Flush() { return true; }
			virtual void Unload() = 0;

		protected:
			static CommandBuffer* (*CreateFunc)();
		};
	}
}