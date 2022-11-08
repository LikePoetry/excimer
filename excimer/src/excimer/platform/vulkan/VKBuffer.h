#pragma once
#include "VK.h"
#include <vulkan/vk_mem_alloc.h>

namespace Excimer
{
	namespace Graphics
	{
		class VKBuffer
		{
		public:
			VKBuffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperyFlags, uint32_t size, const void* data);
			VKBuffer();
			virtual ~VKBuffer();

			void Init(VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperyFlags, uint32_t size, const void* data);
			void Resize(uint32_t size, const void* data);
			void SetData(uint32_t size, const void* data);
			const VkBuffer& GetBuffer() const { return m_Buffer; }

			const VkDescriptorBufferInfo& GetBufferInfo() const { return m_DescriptorBufferInfo; };

			void Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
			void UnMap();
			void Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
			void Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
			void SetUsage(VkBufferUsageFlags flags) { m_UsageFlags = flags; }
			void SetMemoryProperyFlags(VkBufferUsageFlags flags) { m_MemoryPropertyFlags = flags; }
			void Destroy(bool deletionQueue = false);

		protected:
			VkBuffer m_Buffer{};
			VkDeviceMemory m_Memory{};
			VkDescriptorBufferInfo m_DescriptorBufferInfo{};
			VkDeviceSize m_Size = 0;
			VkDeviceSize m_Alignment = 0;
			VkBufferUsageFlags m_UsageFlags;
			VkMemoryPropertyFlags m_MemoryPropertyFlags;
			void* m_Mapped = nullptr;

			VmaAllocation m_Allocation{};
			VmaAllocation m_MappedAllocation{};
		};
	}
}