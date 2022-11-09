#pragma once
#include "excimer/graphics/rhi/RenderDevice.h"

namespace Excimer
{
	namespace Graphics
	{
		class VKRenderDevice:public RenderDevice
		{
		public:
			VKRenderDevice() = default;
			~VKRenderDevice() = default;

			void Init() override;

			static void MakeDefault();

		protected:
			static RenderDevice* CreateFuncVulkan();
		};
	}
}