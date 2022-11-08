#include "hzpch.h"
#include "VKRenderer.h"
#include "excimer/core/Application.h"

namespace Excimer
{
	namespace Graphics
	{
		VKContext::DeletionQueue VKRenderer::s_DeletionQueue[3] = {};

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
