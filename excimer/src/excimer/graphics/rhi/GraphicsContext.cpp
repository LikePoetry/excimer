#include "hzpch.h"
#include "GraphicsContext.h"
#include "excimer/platform/vulkan/VKFunctions.h"


namespace Excimer
{
	namespace Graphics
	{
		GraphicsContext* (*GraphicsContext::CreateFunc)() = nullptr;

		RenderAPI GraphicsContext::s_RenderAPI;

		GraphicsContext* GraphicsContext::Create()
		{
			EXCIMER_ASSERT(CreateFunc, "No GraphicsContext Create Function");
			return CreateFunc();
		}

		GraphicsContext::~GraphicsContext()
		{
		}

		void GraphicsContext::SetRenderAPI(RenderAPI api)
		{
			s_RenderAPI = api;

			switch (s_RenderAPI)
			{
			case RenderAPI::VULKAN:
				Graphics::Vulkan::MakeDefault();
				break;
			default:
				break;
			}
		}
	}
}