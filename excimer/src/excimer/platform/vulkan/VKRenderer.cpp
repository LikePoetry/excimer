#include "hzpch.h"
#include "VKRenderer.h"
#include "excimer/core/Application.h"

namespace Excimer
{
	namespace Graphics
	{

		VKRenderer::~VKRenderer()
		{
			// DescriptorPool deleted by VKContext
		}

		void VKRenderer::MakeDefault()
		{
			CreateFunc = CreateFuncVulkan;
		}

		Renderer* VKRenderer::CreateFuncVulkan()
		{
			return new VKRenderer();
		}
	}
}
