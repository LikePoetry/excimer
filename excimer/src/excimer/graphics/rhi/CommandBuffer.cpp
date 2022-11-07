#include "hzpch.h"
#include "CommandBuffer.h"

namespace Excimer
{
	namespace Graphics
	{
		CommandBuffer* (*CommandBuffer::CreateFunc)() = nullptr;

		CommandBuffer* CommandBuffer::Create()
		{
			EXCIMER_ASSERT(CreateFunc, "No CommandBuffer Create Function");

			return CreateFunc();
		}
	}
}