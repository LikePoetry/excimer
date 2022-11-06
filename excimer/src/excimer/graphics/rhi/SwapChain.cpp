#include "hzpch.h"
#include "SwapChain.h"
#include "excimer/core/ExLog.h"
#include "excimer/core/Core.h"

namespace Excimer
{
	namespace Graphics
	{
		SwapChain* (*SwapChain::CreateFunc)(uint32_t, uint32_t) = nullptr;

		SwapChain* SwapChain::Create(uint32_t width,uint32_t  height)
		{
			EXCIMER_ASSERT(CreateFunc, "No SwapChain Create Function");

			return CreateFunc(width, height);
		}
	}
}