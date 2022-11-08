#include "hzpch.h"
#include "excimer/core/Profiler.h"
#include "VKBuffer.h"
#include "VKRenderer.h"
#include "VKUtilities.h"

#include "VKDevice.h"

namespace Excimer
{
	namespace Graphics
	{
		VKBuffer::VKBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t size, const void* data)
			: m_Size(size)
		{
			Init(usage, memoryPropertyFlags, size, data);
		}

		VKBuffer::VKBuffer()
			: m_Size(0)
		{
		}

		VKBuffer::~VKBuffer()
		{
			EXCIMER_PROFILE_FUNCTION();
			if (m_Buffer)
			{
				VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

				auto buffer = m_Buffer;

				auto alloc = m_Allocation;
				deletionQueue.PushFunction([buffer, alloc]
					{ vmaDestroyBuffer(VKDevice::Get().GetAllocator(), buffer, alloc); });
			}
		}

		void VKBuffer::Destroy(bool deletionQueue)
		{
			EXCIMER_PROFILE_FUNCTION();
			if (m_Buffer)
			{
				VKContext::DeletionQueue& currentDeletionQueue = VKRenderer::GetCurrentDeletionQueue();

				auto buffer = m_Buffer;

				if (deletionQueue)
				{
					auto alloc = m_Allocation;
					currentDeletionQueue.PushFunction([buffer, alloc]
						{ vmaDestroyBuffer(VKDevice::Get().GetAllocator(), buffer, alloc); });
				}
				else
				{
					vmaDestroyBuffer(VKDevice::Get().GetAllocator(), buffer, m_Allocation);
				}
			}

			m_Buffer = VK_NULL_HANDLE;
		}

		void VKBuffer::Init(VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, uint32_t size, const void* data)
		{
			EXCIMER_PROFILE_FUNCTION();

			m_UsageFlags = usage;
			m_MemoryPropertyFlags = memoryPropertyFlags;
			m_Size = size;

			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			bool isMappable = (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;

			VmaAllocationCreateInfo vmaAllocInfo = {};
			vmaAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			vmaAllocInfo.flags = isMappable ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT : 0;
			vmaAllocInfo.preferredFlags = memoryPropertyFlags;

			vmaCreateBuffer(VKDevice::Get().GetAllocator(), &bufferInfo, &vmaAllocInfo, &m_Buffer, &m_Allocation, nullptr);

			if (data != nullptr)
				SetData(size, data);
		}

		void VKBuffer::SetData(uint32_t size, const void* data)
		{
			EXCIMER_PROFILE_FUNCTION();
			Map(size, 0);
			memcpy(m_Mapped, data, size);
			UnMap();
		}

		void VKBuffer::Resize(uint32_t size, const void* data)
		{
			auto usage = m_UsageFlags;

			if (m_Buffer)
			{
				vmaDestroyBuffer(VKDevice::Get().GetAllocator(), m_Buffer, m_Allocation);
			}

			Init(usage, m_MemoryPropertyFlags, size, data);
		}

		void VKBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
		{
			EXCIMER_PROFILE_FUNCTION();
			VkResult res = static_cast<VkResult>(vmaMapMemory(VKDevice::Get().GetAllocator(), m_Allocation, (void**)&m_Mapped));

			if (res != VK_SUCCESS)
				EXCIMER_LOG_CRITICAL("[VULKAN] Failed to map buffer");
		}

		void VKBuffer::UnMap()
		{
			EXCIMER_PROFILE_FUNCTION();

			if (m_Mapped)
			{
				vmaUnmapMemory(VKDevice::Get().GetAllocator(), m_Allocation);
				m_Mapped = nullptr;
			}
		}

		void VKBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
		{
			vmaFlushAllocation(VKDevice::Get().GetAllocator(), m_Allocation, offset, size);
		}

		void VKBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
		{
			EXCIMER_PROFILE_FUNCTION();
			vmaInvalidateAllocation(VKDevice::Get().GetAllocator(), m_Allocation, offset, size);
		}

	}
}