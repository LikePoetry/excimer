#include "hzpch.h"
#include "VKRenderDevice.h"

namespace Excimer
{
	namespace Graphics
	{
        void VKRenderDevice::Init()
        {
        }

        void VKRenderDevice::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
        }

        RenderDevice* VKRenderDevice::CreateFuncVulkan()
        {
            return new VKRenderDevice();
        }
	}
}