#include "hzpch.h"
#include "GraphicsContext.h"

namespace Excimer
{
	namespace Graphics
	{
		GraphicsContext* (*GraphicsContext::CreateFunc)() = nullptr;

		RenderAPI GraphicsContext::s_RenderAPI;

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