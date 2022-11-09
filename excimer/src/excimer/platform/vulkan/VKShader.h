#pragma once

#include "VK.h"
#include "excimer/graphics/rhi/Shader.h"
#include "excimer/graphics/rhi/DescriptorSet.h"
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"

namespace Excimer
{
    namespace Graphics
    {
        class VKShader : public Shader
        {
        public:
            VKShader(const std::string& filePath);
            VKShader(const uint32_t* vertData, uint32_t vertDataSize, const uint32_t* fragData, uint32_t fragDataSize);
            VKShader(const uint32_t* compData, uint32_t compDataSize);
            ~VKShader();

            bool Init();
            void Unload();

            VkPipelineShaderStageCreateInfo* GetShaderStages() const;
            uint32_t GetStageCount() const;

            void Bind() const override {};
            void Unbind() const override {};

            const std::vector<ShaderType> GetShaderTypes() const override
            {
                return std::vector<ShaderType>();
            };

            const std::string& GetName() const override
            {
                return m_Name;
            }
            const std::string& GetFilePath() const override
            {
                return m_FilePath;
            };

            bool IsCompiled() const override
            {
                return m_Compiled;
            }

            bool IsCompute()
            {
                if (m_ShaderTypes.size() > 0)
                    return m_ShaderTypes[0] == ShaderType::COMPUTE;

                return false;
            }

            PushConstant* GetPushConstant(uint32_t index) override
            {
                EXCIMER_ASSERT(index < m_PushConstants.size(), "Push constants out of bounds");
                return &m_PushConstants[index];
            }

            std::vector<PushConstant>& GetPushConstants() override { return m_PushConstants; }
            VkDescriptorSetLayout* GetDescriptorLayout(int id)
            {
                return &m_DescriptorSetLayouts[id];
            };

            VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

            DescriptorSetInfo GetDescriptorInfo(uint32_t index) override
            {
                if (m_DescriptorInfos.find(index) != m_DescriptorInfos.end())
                {
                    return m_DescriptorInfos[index];
                }

                EXCIMER_LOG_WARN("DescriptorDesc not found. Index = {0}", index);
                return DescriptorSetInfo();
            }

            const std::vector<DescriptorLayoutInfo>& GetDescriptorLayout() const { return m_DescriptorLayoutInfo; }
            const std::vector<VkDescriptorSetLayout>& GetDescriptorLayouts() const { return m_DescriptorSetLayouts; }
            void BindPushConstants(Graphics::CommandBuffer* commandBuffer, Graphics::Pipeline* pipeline) override;

            static void PreProcess(const std::string& source, std::map<ShaderType, std::string>* sources);
            static void ReadShaderFile(const std::vector<std::string>& lines, std::map<ShaderType, std::string>* shaders);

            static void MakeDefault();

            void* GetHandle() const override
            {
                return nullptr;
            }

            const std::vector<VkVertexInputAttributeDescription>& GetVertexInputAttributeDescription() const { return m_VertexInputAttributeDescriptions; }
            const uint32_t GetVertexInputStride() const { return m_VertexInputStride; }

        protected:
            static Shader* CreateFuncVulkan(const std::string&);
            static Shader* CreateFromEmbeddedFuncVulkan(const uint32_t* vertData, uint32_t vertDataSize, const uint32_t* fragData, uint32_t fragDataSize);
            static Shader* CreateCompFromEmbeddedFuncVulkan(const uint32_t* compData, uint32_t compDataSize);
            void LoadFromData(const uint32_t* data, uint32_t size, ShaderType type, int currentShaderStage);
            void CreatePipelineLayout();

        private:
            std::unordered_map<uint32_t, DescriptorSetInfo> m_DescriptorInfos;

            VkPipelineShaderStageCreateInfo* m_ShaderStages;
            uint32_t m_StageCount = 0;
            std::string m_Name;
            std::string m_FilePath;
            std::string m_Source;
            std::vector<ShaderType> m_ShaderTypes;
            bool m_Compiled = false;

            std::vector<VkVertexInputAttributeDescription> m_VertexInputAttributeDescriptions;
            uint32_t m_VertexInputStride = 0;

            VkPipelineLayout m_PipelineLayout;
            std::vector<PushConstant> m_PushConstants;
            std::vector<Graphics::DescriptorLayoutInfo> m_DescriptorLayoutInfo;
            std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
        };
    }
}
