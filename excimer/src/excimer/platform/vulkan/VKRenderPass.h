#pragma once
#include "VKDevice.h"
#include "excimer/graphics/rhi/RenderPass.h"

namespace Excimer
{
    namespace Graphics
    {
        class VKRenderPass : public RenderPass
        {
        public:
            VKRenderPass(const RenderPassDesc& renderPassDesc);
            ~VKRenderPass();

            bool Init(const RenderPassDesc& renderPassDesc);
            void BeginRenderpass(CommandBuffer* commandBuffer, float* clearColour, Framebuffer* frame, SubPassContents contents, uint32_t width, uint32_t height) const override;
            void EndRenderpass(CommandBuffer* commandBuffer) override;

            const VkRenderPass& GetHandle() const { return m_RenderPass; };
            int GetAttachmentCount() const override { return m_ClearCount; };
            int GetColourAttachmentCount() const { return m_ColourAttachmentCount; }

            static void MakeDefault();

        protected:
            static RenderPass* CreateFuncVulkan(const RenderPassDesc& renderPassDesc);

        private:
            VkRenderPass m_RenderPass;
            VkClearValue* m_ClearValue;
            int m_ClearCount;
            int m_ColourAttachmentCount;
            bool m_DepthOnly;
            bool m_ClearDepth;
            bool m_SwapchainTarget;
        };
    }
}