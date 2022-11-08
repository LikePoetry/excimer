#pragma once
#include "Defintions.h"

namespace Excimer
{
	namespace Graphics
	{
		struct DescriptorSetInfo
		{
			std::vector<Descriptor> descriptor;
		};

		class DescriptorSet
		{
		public:
			virtual ~DescriptorSet() = default;
			static DescriptorSet* Create(const DescriptorDesc& desc);

			virtual void Update() = 0;
			virtual void SetDynamicOffset(uint32_t offset) = 0;
			virtual uint32_t GetDynamicOffset() const = 0;
			virtual void SetTexture(const std::string& name, Texture** texture, uint32_t textureCount, TextureType textureType = TextureType(0)) = 0;
			virtual void SetTexture(const std::string& name, Texture* texture, uint32_t mipIndex = 0, TextureType textureType = TextureType(0)) = 0;
			virtual void SetBuffer(const std::string& name, UniformBuffer* buffer) = 0;
			virtual Graphics::UniformBuffer* GetUnifromBuffer(const std::string& name) = 0;
			virtual void SetUniform(const std::string& bufferName, const std::string& uniformName, void* data) = 0;
			virtual void SetUniform(const std::string& bufferName, const std::string& uniformName, void* data, uint32_t size) = 0;
			virtual void SetUniformBufferData(const std::string& bufferName, void* data) = 0;
			virtual void TransitionImages(CommandBuffer* commandBuffer = nullptr) { }
			virtual void SetUniformDynamic(const std::string& bufferName, uint32_t size) { }

		protected:
			static DescriptorSet* (*CreateFunc)(const DescriptorDesc&);

		};
	}
}