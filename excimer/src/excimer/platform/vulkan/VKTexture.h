#pragma once
#include "excimer/graphics/rhi/Texture.h"

#include "VKContext.h"
#include "VKCommandBuffer.h"
#include <vulkan/vk_mem_alloc.h>

namespace Excimer
{
	namespace Graphics
	{
		class VKTexture2D:public Texture2D
		{
		public:
			VKTexture2D(TextureDesc parameters, uint32_t width, uint32_t height);
			VKTexture2D(uint32_t width, uint32_t height, void* data, TextureDesc parameters = TextureDesc(), TextureLoadOptions loadOptions = TextureLoadOptions());
			VKTexture2D(const std::string& name, const std::string& filename, TextureDesc parameters = TextureDesc(), TextureLoadOptions loadOptions = TextureLoadOptions());
			VKTexture2D(VkImage image, VkImageView imageView, VkFormat format, uint32_t width, uint32_t height);
			~VKTexture2D();

			void DeleteResources();

			virtual void* GetHandle() const override
			{
				return (void*)this;
			}

			void BuildTexture();

			void UpdateDescriptor();
			bool Load();

			VkImageView GetMipImageView(uint32_t mip);

			void TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer = nullptr);

			

			static void MakeDefault();

		protected:
			static Texture2D* CreateFuncVulkan(TextureDesc, uint32_t, uint32_t);
			static Texture2D* CreateFromSourceFuncVulkan(uint32_t, uint32_t, void*, TextureDesc, TextureLoadOptions);
			static Texture2D* CreateFromFileFuncVulkan(const std::string&, const std::string&, TextureDesc, TextureLoadOptions);
		private:
			std::string m_Name;
			std::string m_FileName;
			uint32_t m_Width{}, m_Height{};
			uint32_t m_MipLevels = 1;
			uint8_t* m_Data = nullptr;

			TextureDesc m_Parameters;
			TextureLoadOptions m_LoadOptions;

			RHIFormat m_Format;
			VkFormat m_VKFormat = VK_FORMAT_R8G8B8A8_UNORM;

			VkImage m_TextureImage{};
			VkImageLayout m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkDeviceMemory m_TextureImageMemory{};
			VkImageView m_TextureImageView;
			VkSampler m_TextureSampler{};
			VkDescriptorImageInfo m_Descriptor{};

			std::unordered_map<uint32_t, VkImageView> m_MipImageViews;

			VmaAllocation m_Allocation{};

			bool m_DeleteImage = true;
		};
	}
}