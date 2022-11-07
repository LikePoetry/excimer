#pragma once

#include "VK.h"
#include "VKFence.h"
#include "excimer/core/Reference.h"
#include "excimer/graphics/rhi/CommandBuffer.h"

namespace Excimer
{
	namespace Graphics
	{
		enum class CommandBufferState :uint8_t
		{
			Idle,
			Recording,
			Ended,
			Submitted
		};

		class VKCommandBuffer:public CommandBuffer
		{
		public:
			VKCommandBuffer();
			VKCommandBuffer(VkCommandBuffer commandBuffer);
			~VKCommandBuffer();

			bool Init(bool primary) override;
			bool Init(bool primary, VkCommandPool commandPool);
			void Unload() override;
			void Reset();
			bool Flush() override;
			bool Wait();

			VkCommandBuffer GetHandle() const { return m_CommandBuffer; };
			CommandBufferState GetState() const { return m_State; }
			VkSemaphore GetSemaphore() const { return m_Semaphore; }

			static void MakeDefault();
		protected:
			static CommandBuffer* CreateFuncVulkan();

		private:
			VkCommandBuffer m_CommandBuffer;
			VkCommandPool m_CommandPool;
			bool m_Primary;
			CommandBufferState m_State;
			SharedPtr<VKFence> m_Fence;
			VkSemaphore m_Semaphore;


		};
	}
}