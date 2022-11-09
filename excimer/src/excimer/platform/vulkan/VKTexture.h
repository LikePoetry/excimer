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

			void Bind(uint32_t slot = 0) const override {};
			void Unbind(uint32_t slot = 0) const override {};

			virtual void SetData(const void* pixels) override {};

			virtual void* GetHandle() const override
			{
				return (void*)this;
			}

			inline uint32_t GetWidth(uint32_t mip = 0) const override
			{
				return m_Width >> mip;
			}

			inline uint32_t GetHeight(uint32_t mip = 0) const override
			{
				return m_Height >> mip;
			}

			uint32_t GetMipMapLevels() const override
			{
				return m_MipLevels;
			}

			inline const std::string& GetName() const override
			{
				return m_Name;
			}

			inline const std::string& GetFilepath() const override
			{
				return m_FileName;
			}

			TextureType GetType() const override
			{
				return TextureType::COLOUR;
			}

			RHIFormat GetFormat() const override
			{
				return m_Format;
			}

			void SetName(const std::string& name) override
			{
				m_Name = name;
			}


			void BuildTexture();
			void Resize(uint32_t width, uint32_t height) override;

			const VkDescriptorImageInfo* GetDescriptor() const
			{
				return &m_Descriptor;
			}

			void SetImageLayout(VkImageLayout layout)
			{
				m_ImageLayout = layout;
				UpdateDescriptor();
			}

			VkDescriptorImageInfo& GetDescriptorRef()
			{
				return m_Descriptor;
			}

			void* GetDescriptorInfo() const override
			{
				return (void*)GetDescriptor();
			}

			virtual void* GetImageHandle() const override
			{
				return (void*)m_TextureImage;
			}

			void UpdateDescriptor();

			bool Load();

			VkImage GetImage() const
			{
				return m_TextureImage;
			};
			VkDeviceMemory GetDeviceMemory() const
			{
				return m_TextureImageMemory;
			}
			VkImageView GetImageView() const
			{
				return m_TextureImageView;
			}
			VkSampler GetSampler() const
			{
				return m_TextureSampler;
			}

			VkFormat GetVKFormat()
			{
				return m_VKFormat;
			}

			VkImageView GetMipImageView(uint32_t mip);

			VkImageLayout GetImageLayout() const { return m_ImageLayout; }
			const TextureDesc& GetTextureParameters() const { return m_Parameters; }
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