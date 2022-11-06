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

			static void MakeDefault();

		protected:
			static Renderer* CreateFuncVulkan();
		};
	}
}