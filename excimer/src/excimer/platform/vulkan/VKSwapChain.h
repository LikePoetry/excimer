#pragma once
#include "excimer/core/Core.h"
#include "excimer/graphics/rhi/SwapChain.h"

#include "VK.h"
#include "VKTexture.h"
#include "VKCommandPool.h"
#include "VKCommandBuffer.h"


#define MAX_SWAPCHAIN_BUFFERS 3
namespace Excimer
{
	class Window;
	namespace Graphics
	{
		struct FrameData
		{
			VkSemaphore PresentSemaphore = VK_NULL_HANDLE;
			SharedPtr<VKCommandPool> CommandPool;
			SharedPtr<VKCommandBuffer> MainCommandBuffer;
		};

		class VKSwapChain:public SwapChain
		{
		public:
			VKSwapChain(uint32_t width, uint32_t height);
			~VKSwapChain();

			bool Init(bool vsync) override;
			bool Init(bool vsync, Window* windowHandle) override;
			void CreateFrameData();
			void AcquireNextImage();
			void OnResize(uint32_t width, uint32_t height, bool forceResize = false, Window* windowHandle = nullptr);

			uint32_t GetCurrentBufferIndex() const override { return m_CurrentBuffer; }

			VkSurfaceKHR CreatePlatformSurface(VkInstance vkInstance, Window* window);

			static void MakeDefault();

		protected:
			static SwapChain* CreateFuncVulkan(uint32_t width, uint32_t height);

		private:
			FrameData m_Frames[MAX_SWAPCHAIN_BUFFERS];

			void FindImageFormatAndColourSpace();

			std::vector<Texture2D*> m_SwapChainBuffers;

			uint32_t m_CurrentBuffer = 0;
			uint32_t m_AcquireImageIndex = 0;
			uint32_t m_Width;
			uint32_t m_Height;

			uint32_t m_SwapChainBufferCount;
			bool m_VSyncEnabled = false;

			VkSwapchainKHR m_SwapChain;
			VkSwapchainKHR m_OldSwapChain;
			VkSurfaceKHR m_Surface;
			VkFormat m_ColourFormat;
			VkColorSpaceKHR m_ColourSpace;
		};
	}
}