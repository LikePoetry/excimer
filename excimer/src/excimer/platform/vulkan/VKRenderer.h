#pragma once
#include "VK.h"
#include "VKContext.h"
#include "VKSwapChain.h"
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

			static VKSwapChain* GetMainSwapChain()
			{
				return static_cast<VKSwapChain*>(Renderer::GetMainSwapChain());
			}

			static VKContext::DeletionQueue& GetCurrentDeletionQueue()
			{
				return s_DeletionQueue[(s_Instance && Application::Get().GetWindow()) ? GetMainSwapChain()->GetCurrentBufferIndex() : 0];
			}

			template <typename F>
			static void PushDeletionFunction(F&& function)
			{
				GetCurrentDeletionQueue().PushFunction(function);
			}

			static void MakeDefault();

		protected:
			static Renderer* CreateFuncVulkan();

		private:

			static VKContext::DeletionQueue s_DeletionQueue[3];
		};
	}
}