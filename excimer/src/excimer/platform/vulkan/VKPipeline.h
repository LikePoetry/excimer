#pragma once
#include "VK.h"
#include "excimer/graphics/rhi/Pipeline.h"
#include "VKDescriptorSet.h"
#include "VKFramebuffer.h"
#include "VKRenderPass.h"
#include "excimer/core/Reference.h"

namespace Excimer
{
    class Shader;

    namespace Graphics
    {
        class VKCommandBuffer;
        class VKFramebuffer;

        class VKPipeline : public Pipeline
        {
        public:
            VKPipeline(const PipelineDesc& pipelineDesc);
            ~VKPipeline();

            bool Init(const PipelineDesc& pipelineDesc);
            void Bind(CommandBuffer* commandBuffer, uint32_t layer) override;
            void End(CommandBuffer* commandBuffer) override;

            const VkPipelineLayout& GetPipelineLayout() const
            {
                return m_PipelineLayout;
            };

            const VkPipeline& GetPipeline() const
            {
                return m_Pipeline;
            }

            Shader* GetShader() const override
            {
                return m_Shader.get();
            }

            void CreateFramebuffers();
            void ClearRenderTargets(CommandBuffer* commandBuffer) override;
            void TransitionAttachments();
            bool IsCompute() const { return m_Compute; }

            static void MakeDefault();

        protected:
            static Pipeline* CreateFuncVulkan(const PipelineDesc& pipelineDesc);

        private:
            SharedPtr<Shader> m_Shader;
            SharedPtr<RenderPass> m_RenderPass;
            std::vector<SharedPtr<VKFramebuffer>> m_Framebuffers;

            VkPipelineLayout m_PipelineLayout;
            VkPipeline m_Pipeline;
            bool m_DepthBiasEnabled;
            float m_DepthBiasConstant;
            float m_DepthBiasSlope;
            bool m_Compute = false;
        };
    }
}
