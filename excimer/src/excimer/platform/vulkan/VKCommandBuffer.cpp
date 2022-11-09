#include "hzpch.h"
#include "VKCommandBuffer.h"
#include "VKDevice.h"
#include "VKInitialisers.h"
#include "excimer/core/Profiler.h"
#include "VKUtilities.h"
#include "VKRenderPass.h"
#include "VKFramebuffer.h"


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

        void VKCommandBuffer::BeginRecording()
        {
            EXCIMER_PROFILE_FUNCTION();
            EXCIMER_ASSERT(m_Primary, "BeginRecording() called from a secondary command buffer!");

            m_State = CommandBufferState::Recording;
            VkCommandBufferBeginInfo beginCreateInfo = VKInitialisers::CommandBufferBeginInfo();
            beginCreateInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginCreateInfo));
        }

        void VKCommandBuffer::BeginRecordingSecondary(RenderPass* renderPass, Framebuffer* framebuffer)
        {
            EXCIMER_PROFILE_FUNCTION();
            EXCIMER_ASSERT(!m_Primary, "BeginRecordingSecondary() called from a primary command buffer!");
            m_State = CommandBufferState::Recording;

            VkCommandBufferInheritanceInfo inheritanceInfo = VKInitialisers::CommandBufferInheritanceInfo();
            inheritanceInfo.subpass = 0;
            inheritanceInfo.renderPass = static_cast<VKRenderPass*>(renderPass)->GetHandle();
            inheritanceInfo.framebuffer = static_cast<VKFramebuffer*>(framebuffer)->GetFramebuffer();

            VkCommandBufferBeginInfo beginCreateInfo = VKInitialisers::CommandBufferBeginInfo();
            beginCreateInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
            beginCreateInfo.pInheritanceInfo = &inheritanceInfo;

            VK_CHECK_RESULT(vkBeginCommandBuffer(m_CommandBuffer, &beginCreateInfo));
        }

        void VKCommandBuffer::EndRecording()
        {
            EXCIMER_PROFILE_FUNCTION();
            EXCIMER_ASSERT(m_State == CommandBufferState::Recording, "CommandBuffer ended before started recording");

            if (m_BoundPipeline)
                m_BoundPipeline->End(this);

            m_BoundPipeline = nullptr;
            TracyVkCollect(VKDevice::Get().GetTracyContext(), m_CommandBuffer);

            VK_CHECK_RESULT(vkEndCommandBuffer(m_CommandBuffer));
            m_State = CommandBufferState::Ended;
        }

        void VKCommandBuffer::Execute(VkPipelineStageFlags flags, VkSemaphore waitSemaphore, bool waitFence)
        {
            EXCIMER_PROFILE_FUNCTION();
            EXCIMER_ASSERT(m_Primary, "Used Execute on secondary command buffer!");
            EXCIMER_ASSERT(m_State == CommandBufferState::Ended, "CommandBuffer executed before ended recording");
            uint32_t waitSemaphoreCount = waitSemaphore ? 1 : 0, signalSemaphoreCount = m_Semaphore ? 1 : 0;

            VkSubmitInfo submitInfo = VKInitialisers::SubmitInfo();
            submitInfo.waitSemaphoreCount = waitSemaphoreCount;
            submitInfo.pWaitSemaphores = &waitSemaphore;
            submitInfo.pWaitDstStageMask = &flags;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &m_CommandBuffer;
            submitInfo.signalSemaphoreCount = signalSemaphoreCount;
            submitInfo.pSignalSemaphores = &m_Semaphore;

            m_Fence->Reset();

            {
                EXCIMER_PROFILE_SCOPE("vkQueueSubmit");
                VK_CHECK_RESULT(vkQueueSubmit(VKDevice::Get().GetGraphicsQueue(), 1, &submitInfo, m_Fence->GetHandle()));
            }

            m_State = CommandBufferState::Submitted;
        }

        void VKCommandBuffer::ExecuteSecondary(CommandBuffer* primaryCmdBuffer)
        {
            EXCIMER_PROFILE_FUNCTION();
            EXCIMER_ASSERT(!m_Primary, "Used ExecuteSecondary on primary command buffer!");
            m_State = CommandBufferState::Submitted;

            vkCmdExecuteCommands(static_cast<VKCommandBuffer*>(primaryCmdBuffer)->GetHandle(), 1, &m_CommandBuffer);
        }

        void VKCommandBuffer::BindPipeline(Pipeline* pipeline)
        {
            if (pipeline != m_BoundPipeline)
            {
                if (m_BoundPipeline)
                    m_BoundPipeline->End(this);

                pipeline->Bind(this);
                m_BoundPipeline = pipeline;
            }
        }

        void VKCommandBuffer::UnBindPipeline()
        {
            if (m_BoundPipeline)
                m_BoundPipeline->End(this);
            m_BoundPipeline = nullptr;
        }

        void VKCommandBuffer::UpdateViewport(uint32_t width, uint32_t height, bool flipViewport)
        {
            EXCIMER_PROFILE_FUNCTION();
            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(width);
            viewport.height = static_cast<float>(height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            if (flipViewport)
            {
                viewport.width = (float)width;
                viewport.height = -(float)height;
                viewport.x = 0;
                viewport.y = (float)height;
            }

            VkRect2D scissor = {};
            scissor.offset = { 0, 0 };
            scissor.extent = { width, height };

            vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);
        }

        bool VKCommandBuffer::Wait()
        {
            EXCIMER_PROFILE_FUNCTION();
            EXCIMER_ASSERT(m_State == CommandBufferState::Submitted, "");

            m_Fence->WaitAndReset();
            m_State = CommandBufferState::Idle;

            return true;
        }

        void VKCommandBuffer::Submit()
        {
            EXCIMER_PROFILE_FUNCTION();
            Execute(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, nullptr, false);
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