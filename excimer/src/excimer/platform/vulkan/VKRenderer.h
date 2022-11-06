#pragma once
#include "VK.h"
#include "VKContext.h"
#include "excimer/graphics/rhi/Renderer.h"

#define NUM_SEMAPHORES 10

namespace Excimer
{
	namespace Graphics
	{
		class EXCIMER_EXPORT VKRenderer:public Renderer
		{
		public:
			VKRenderer() = default;
			~VKRenderer();

			static VKContext* GetGraphicsContext()
			{
				return static_cast<VKContext*>(s_Instance->GetGraphicsContext());
			}

			static void MakeDefault();

		protected:
			static Renderer* CreateFuncVulkan();
		};
	}
}