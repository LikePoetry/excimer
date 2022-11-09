#pragma once

#include <vector>
#include "vulkan/vulkan.h"

namespace Excimer
{
	namespace VKInitialisers
	{

        inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
            VkCommandPool commandPool,
            VkCommandBufferLevel level,
            uint32_t bufferCount)
        {
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool = commandPool;
            commandBufferAllocateInfo.level = level;
            commandBufferAllocateInfo.commandBufferCount = bufferCount;
            return commandBufferAllocateInfo;
        }

        inline VkCommandBufferBeginInfo CommandBufferBeginInfo()
        {
            VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
            cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            return cmdBufferBeginInfo;
        }

        inline VkCommandBufferInheritanceInfo CommandBufferInheritanceInfo()
        {
            VkCommandBufferInheritanceInfo cmdBufferInheritanceInfo = {};
            cmdBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
            return cmdBufferInheritanceInfo;
        }

        inline VkFramebufferCreateInfo FramebufferCreateInfo()
        {
            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            return framebufferCreateInfo;
        }

        inline VkSemaphoreCreateInfo SemaphoreCreateInfo()
        {
            VkSemaphoreCreateInfo semaphoreCreateInfo = {};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            return semaphoreCreateInfo;
        }

        inline VkSubmitInfo SubmitInfo()
        {
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pNext = VK_NULL_HANDLE;
            return submitInfo;
        }

        inline VkRenderPassBeginInfo renderPassBeginInfo()
        {
            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            return renderPassBeginInfo;
        }

        inline VkRenderPassCreateInfo RenderPassCreateInfo()
        {
            VkRenderPassCreateInfo renderPassCreateInfo = {};
            renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            return renderPassCreateInfo;
        }

        /** @brief Initialize an image memory barrier with no image transfer ownership */
        inline VkImageMemoryBarrier ImageMemoryBarrier()
        {
            VkImageMemoryBarrier imageMemoryBarrier = {};
            imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            return imageMemoryBarrier;
        }
	}
}