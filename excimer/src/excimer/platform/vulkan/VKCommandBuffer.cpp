#include "hzpch.h"
#include "VKCommandBuffer.h"
#include "VKDevice.h"
#include "VKInitialisers.h"
#include "excimer/core/Profiler.h"
#include "VKUtilities.h"

#include <Tracy/TracyVulkan.hpp>
#include <excimer/platform/vulkan/VKUtilities.h>

namespace Excimer
{
    namespace Graphics
    {
        VKCommandBuffer::VKCommandBuffer()
            : m_CommandBuffer(nullptr)
            , m_CommandPool(nullptr)
            , m_Semaphore(nullptr)
            , m_Primary(false)
            , m_State(CommandBufferState::Idle)
        {

        }

        VKCommandBuffer::VKCommandBuffer(VkCommandBuffer commandBuffer)
            : m_CommandBuffer(commandBuffer)
            , m_CommandPool(nullptr)
            , m_Semaphore(nullptr)
            , m_Primary(true)
            , m_State(CommandBufferState::Idle)
        {
        }

        VKCommandBuffer::~VKCommandBuffer()
        {
            Unload();
        }

        bool VKCommandBuffer::Init(bool primary)
        {
            EXCIMER_PROFILE_FUNCTION();
            m_Primary = primary;
            m_CommandPool= VKDevice::Get().GetCommandPool()->GetHandle();
            VkCommandBufferAllocateInfo cmdBufferCreateInfo = VKInitialisers::CommandBufferAllocateInfo(m_CommandPool, primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1);

            VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().GetDevice(), &cmdBufferCreateInfo, &m_CommandBuffer));

            VkSemaphoreCreateInfo semaphoreInfo = VKInitialisers::SemaphoreCreateInfo();
            semaphoreInfo.pNext = nullptr;

            VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().GetDevice(), &semaphoreInfo, nullptr, &m_Semaphore));
            m_Fence = CreateSharedPtr<VKFence>(false);

            return true;
        }

        bool VKCommandBuffer::Init(bool primary, VkCommandPool commandPool)
        {
            EXCIMER_PROFILE_FUNCTION();
            m_Primary = primary;

            m_CommandPool = commandPool;

            VkCommandBufferAllocateInfo cmdBufferCreateInfo = VKInitialisers::CommandBufferAllocateInfo(m_CommandPool, primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1);

            VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().GetDevice(), &cmdBufferCreateInfo, &m_CommandBuffer));

            VkSemaphoreCreateInfo semaphoreInfo = VKInitialisers::SemaphoreCreateInfo();
            semaphoreInfo.pNext = nullptr;

            VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().GetDevice(), &semaphoreInfo, nullptr, &m_Semaphore));
            m_Fence = CreateSharedPtr<VKFence>(false);

            return true;
        }

        void VKCommandBuffer::Unload()
        {
            EXCIMER_PROFILE_FUNCTION();
            VKUtilities::WaitIdle();

            if (m_State == CommandBufferState::Submitted)
                Wait();

            m_Fence = nullptr;
            vkDestroySemaphore(VKDevice::Get().GetDevice(), m_Semaphore, nullptr);
            vkFreeCommandBuffers(VKDevice::Get().GetDevice(), m_CommandPool, 1, &m_CommandBuffer);
        }

        bool VKCommandBuffer::Wait()
        {
            EXCIMER_PROFILE_FUNCTION();
            EXCIMER_ASSERT(m_State == CommandBufferState::Submitted, "");

            m_Fence->WaitAndReset();
            m_State = CommandBufferState::Idle;

            return true;
        }

        void VKCommandBuffer::Reset()
        {
            EXCIMER_PROFILE_FUNCTION();
            VK_CHECK_RESULT(vkResetCommandBuffer(m_CommandBuffer, 0));
        }

        bool VKCommandBuffer::Flush()
        {
            EXCIMER_PROFILE_FUNCTION();
            if (m_State == CommandBufferState::Idle)
                return true;

            VKUtilities::WaitIdle();

            if (m_State == CommandBufferState::Submitted)
                return Wait();

            return true;
        }

        void VKCommandBuffer::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
        }

        CommandBuffer* VKCommandBuffer::CreateFuncVulkan()
        {
            return new VKCommandBuffer();
        }
    }
}