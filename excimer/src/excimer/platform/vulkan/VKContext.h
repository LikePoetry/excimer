#pragma once
#include "excimer/graphics/rhi/GraphicsContext.h"
#include "VK.h"
#include "excimer/core/Reference.h"

#include <deque>

#include <vulkan/vk_mem_alloc.h>

const bool EnableValidationLayers = true;

namespace Excimer
{
	namespace Graphics
	{
		class VKContext :public GraphicsContext
		{
		public:
			VKContext();

			void Init() override;

			static void MakeDefault();

		protected:
			static GraphicsContext* CreateFuncVulkan();

			void CreateInstance();

		};
	}
}