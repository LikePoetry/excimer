#include "hzpch.h"
#include "RenderDevice.h"
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"

namespace Excimer
{
	namespace Graphics
	{
		RenderDevice* (*RenderDevice::CreateFunc)() = nullptr;

		RenderDevice* RenderDevice::s_Instance = nullptr;

		void RenderDevice::Create()
		{
			EXCIMER_ASSERT(CreateFunc, "No RenderDevice Create Function");

			s_Instance = CreateFunc();
		}

		void RenderDevice::Release()
		{
			delete s_Instance;
		};
	}
}