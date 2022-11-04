#include "hzpch.h"
#include "VKContext.h"
#include "excimer/core/Profiler.h"

namespace Excimer
{
	namespace Graphics
	{
		VKContext::VKContext()
		{

		}

		void VKContext::MakeDefault()
		{
			CreateFunc = CreateFuncVulkan;
		}

		GraphicsContext* VKContext::CreateFuncVulkan()
		{
			return new VKContext();
		}

		void VKContext::Init()
		{
			EXCIMER_PROFILE_FUNCTION();
			CreateInstance();


		}

		void VKContext::CreateInstance()
		{
			EXCIMER_PROFILE_FUNCTION();
			if (volkInitialize() != VK_SUCCESS)
			{
				EXCIMER_LOG_CRITICAL("volkInitialize failed");
			}

			if (volkGetInstanceVersion() == 0)
			{
				EXCIMER_LOG_CRITICAL("Could not find loader");
			}

		}
	}
}