#include "hzpch.h"
#include "VKTexture.h"
#include "excimer/core/Profiler.h"
#include "excimer/maths/Maths.h"
#include "VKUtilities.h"
#include "VKDevice.h"


namespace Excimer
{
    namespace Graphics
    {

        static VkImageView CreateImageView(VkImage image, VkFormat format, uint32_t mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectMask, uint32_t layerCount, uint32_t baseArrayLayer = 0, uint32_t baseMipLevel = 0)
        {
            EXCIMER_PROFILE_FUNCTION();
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = viewType;
            viewInfo.format = format;

            viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
            viewInfo.subresourceRange.aspectMask = aspectMask;
            viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
            viewInfo.subresourceRange.levelCount = mipLevels;
            viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
            viewInfo.subresourceRange.layerCount = layerCount;

            VkImageView imageView;
            if (vkCreateImageView(VKDevice::Get().GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
            {
                EXCIMER_LOG_ERROR("Failed to create texture image view!");
            }

            return imageView;
        }

        static VkSampler CreateTextureSampler(VkFilter magFilter = VK_FILTER_LINEAR, VkFilter minFilter = VK_FILTER_LINEAR, float minLod = 0.0f, float maxLod = 1.0f, bool anisotropyEnable = false, float maxAnisotropy = 1.0f, VkSamplerAddressMode modeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
        {
            EXCIMER_PROFILE_FUNCTION();
            VkSampler sampler;
            VkSamplerCreateInfo samplerInfo = {};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = magFilter;
            samplerInfo.minFilter = minFilter;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.addressModeU = modeU;
            samplerInfo.addressModeV = modeV;
            samplerInfo.addressModeW = modeW;
            samplerInfo.maxAnisotropy = maxAnisotropy;
            samplerInfo.anisotropyEnable = anisotropyEnable;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
            samplerInfo.minLod = minLod;
            samplerInfo.maxLod = maxLod;

            if (vkCreateSampler(VKDevice::Get().GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
            {
                EXCIMER_LOG_ERROR("Failed to create texture sampler!");
            }

            return sampler;
        }

        static void CreateImageVma(const VkImageCreateInfo& imageInfo, VkImage& image, VmaAllocation& allocation)
        {
            EXCIMER_PROFILE_FUNCTION();
            VmaAllocationCreateInfo allocInfovma;
            allocInfovma.flags = 0;
            allocInfovma.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfovma.requiredFlags = 0;
            allocInfovma.preferredFlags = 0;
            allocInfovma.memoryTypeBits = 0;
            allocInfovma.pool = nullptr;
            allocInfovma.pUserData = nullptr;
            vmaCreateImage(VKDevice::Get().GetAllocator(), &imageInfo, &allocInfovma, &image, &allocation, nullptr);
        }

        static void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t arrayLayers, VkImageCreateFlags flags, VmaAllocation& allocation)
        {
            EXCIMER_PROFILE_FUNCTION();
            VkImageCreateInfo imageInfo = {};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = imageType;
            imageInfo.extent = { width, height, 1 };
            imageInfo.mipLevels = mipLevels;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.arrayLayers = arrayLayers;

            imageInfo.flags = flags;
            CreateImageVma(imageInfo, image, allocation);
        }

        VKTexture2D::VKTexture2D(TextureDesc parameters, uint32_t width, uint32_t height)
            : m_FileName("NULL")
            , m_TextureImage(VK_NULL_HANDLE)
            , m_TextureImageView(VK_NULL_HANDLE)
            , m_TextureSampler(VK_NULL_HANDLE)
        {
            m_Width = width;
            m_Height = height;
            m_Parameters = parameters;
            m_VKFormat = VKUtilities::FormatToVK(parameters.format, parameters.srgb);
            m_DeleteImage = true;
            m_MipLevels = 1;
            m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);
            m_Flags = m_Parameters.flags;

            BuildTexture();
        }

        VKTexture2D::VKTexture2D(uint32_t width, uint32_t height, void* data, TextureDesc parameters, TextureLoadOptions loadOptions)
            : m_FileName("NULL")
            , m_TextureImage(VK_NULL_HANDLE)
            , m_TextureImageView(VK_NULL_HANDLE)
            , m_TextureSampler(VK_NULL_HANDLE)
        {
            m_Width = width;
            m_Height = height;
            m_Parameters = parameters;
            m_LoadOptions = loadOptions;
            m_Data = static_cast<uint8_t*>(data);
            m_Format = parameters.format;
            m_VKFormat = VKUtilities::FormatToVK(parameters.format, parameters.srgb);
            m_Flags = m_Parameters.flags;

            Load();

            m_TextureImageView = Graphics::CreateImageView(m_TextureImage, VKUtilities::FormatToVK(parameters.format, parameters.srgb), m_MipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);
            m_TextureSampler = Graphics::CreateTextureSampler(VKUtilities::TextureFilterToVK(m_Parameters.magFilter), VKUtilities::TextureFilterToVK(m_Parameters.minFilter), 0.0f, static_cast<float>(m_MipLevels), true, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap));

            UpdateDescriptor();
        }

        VKTexture2D::VKTexture2D(const std::string& name, const std::string& filename, TextureDesc parameters, TextureLoadOptions loadOptions)
            : m_FileName(filename)
            , m_TextureImage(VK_NULL_HANDLE)
            , m_TextureImageView(VK_NULL_HANDLE)
            , m_TextureSampler(VK_NULL_HANDLE)
        {
            m_Parameters = parameters;
            m_LoadOptions = loadOptions;
            m_Format = parameters.format;
            m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);
            m_Flags = m_Parameters.flags;

            m_DeleteImage = Load();

            if (!m_DeleteImage)
                return;

            m_TextureImageView = Graphics::CreateImageView(m_TextureImage, m_VKFormat, m_MipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);
            m_TextureSampler = Graphics::CreateTextureSampler(VKUtilities::TextureFilterToVK(m_Parameters.magFilter), VKUtilities::TextureFilterToVK(m_Parameters.minFilter), 0.0f, static_cast<float>(m_MipLevels), true, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap));

            UpdateDescriptor();
        }

        VKTexture2D::VKTexture2D(VkImage image, VkImageView imageView, VkFormat format, uint32_t width, uint32_t height)
            :m_TextureImage(image)
            , m_TextureImageView(imageView)
            , m_TextureSampler(VK_NULL_HANDLE)
            , m_Width(width)
            , m_Height(height)
            , m_VKFormat(format)
            , m_DeleteImage(false)
            , m_ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
        {
            m_TextureImageMemory = VK_NULL_HANDLE;

            UpdateDescriptor();
        }

        VKTexture2D::~VKTexture2D()
        {
            EXCIMER_PROFILE_FUNCTION();
            DeleteResources();
        }

        void VKTexture2D::DeleteResources()
        {

        }

        void VKTexture2D::UpdateDescriptor()
        {
            m_Descriptor.sampler = m_TextureSampler;
            m_Descriptor.imageView = m_TextureImageView;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        bool VKTexture2D::Load()
        {
            //EXCIMER_PROFILE_FUNCTION();
            //uint32_t bits;
            //uint8_t* pixels;

            //m_Flags |= TextureFlags::Texture_Sampled;

            //if (m_Data == nullptr)
            //{
            //    pixels = Excimer::LoadImageFromFile(m_FileName, &m_Width, &m_Height, &bits);
            //    if (pixels == nullptr)
            //        return false;

            //    m_Parameters.format = BitsToFormat(bits);
            //    m_Format = m_Parameters.format;
            //}
            //else
            //{
            //    if (m_Data == nullptr)
            //        return false;

            //    m_BitsPerChannel = GetBitsFromFormat(m_Parameters.format);
            //    bits = m_BitsPerChannel;
            //    pixels = m_Data;
            //}

            //m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);

            //VkDeviceSize imageSize = VkDeviceSize(m_Width * m_Height * bits / 8);

            //if (!pixels)
            //{
            //    EXCIMER_LOG_CRITICAL("failed to load texture image!");
            //}

            //m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(Maths::Max(m_Width, m_Height)))) + 1;

            //if (!(m_Flags & TextureFlags::Texture_CreateMips))
            //    m_MipLevels = 1;

            //VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<uint32_t>(imageSize), pixels);

            //if (m_Data == nullptr)
            //    delete[] pixels;

            //Graphics::CreateImage(m_Width, m_Height, m_MipLevels, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0, m_Allocation);


            //VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
            //VKUtilities::CopyBufferToImage(stagingBuffer->GetBuffer(), m_TextureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));

            //delete stagingBuffer;

            //if (m_Flags & TextureFlags::Texture_CreateMips)
            //    GenerateMipmaps(m_TextureImage, m_VKFormat, m_Width, m_Height, m_MipLevels);

            //TransitionImage(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            return true;
        }

        void VKTexture2D::TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer)
        {
            EXCIMER_PROFILE_FUNCTION();
            if (newLayout != m_ImageLayout)
                VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, m_ImageLayout, newLayout, m_MipLevels, 1, commandBuffer ? commandBuffer->GetHandle() : nullptr);
            m_ImageLayout = newLayout;
            UpdateDescriptor();
        }

        void VKTexture2D::BuildTexture()
        {
            if (m_Flags & TextureFlags::Texture_CreateMips)
            {
                m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(Maths::Max(m_Width, m_Height)))) + 1;
            }
            Graphics::CreateImage(m_Width, m_Height, m_MipLevels, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0, m_Allocation);


            m_TextureImageView = Graphics::CreateImageView(m_TextureImage, m_VKFormat, m_MipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);
            m_TextureSampler = Graphics::CreateTextureSampler(VKUtilities::TextureFilterToVK(m_Parameters.minFilter), VKUtilities::TextureFilterToVK(m_Parameters.magFilter), 0.0f, static_cast<float>(m_MipLevels), false, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap));

            m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            TransitionImage(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            UpdateDescriptor();

            if (m_Flags & TextureFlags::Texture_MipViews)
            {
                for (uint32_t i = 0; i < m_MipLevels; i++)
                {
                    GetMipImageView(i);
                }
            }
        }

        void VKTexture2D::Resize(uint32_t width, uint32_t height)
        {
            DeleteResources();

            m_Width = width;
            m_Height = height;
            m_TextureImage = VkImage();

            BuildTexture();
        }

        VkImageView VKTexture2D::GetMipImageView(uint32_t mip)
        {
            if (m_MipImageViews.find(mip) == m_MipImageViews.end())
            {
                m_MipImageViews[mip] = CreateImageView(m_TextureImage, m_VKFormat, 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, mip);
            }
            return m_MipImageViews.at(mip);
        }

        void VKTexture2D::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
            CreateFromFileFunc = CreateFromFileFuncVulkan;
            CreateFromSourceFunc = CreateFromSourceFuncVulkan;
        }

        Texture2D* VKTexture2D::CreateFuncVulkan(TextureDesc parameters, uint32_t width, uint32_t height)
        {
            return new VKTexture2D(parameters, width, height);
        }

        Texture2D* VKTexture2D::CreateFromSourceFuncVulkan(uint32_t width, uint32_t height, void* data, TextureDesc parameters, TextureLoadOptions loadoptions)
        {
            return new VKTexture2D(width, height, data, parameters, loadoptions);
        }

        Texture2D* VKTexture2D::CreateFromFileFuncVulkan(const std::string& name, const std::string& filename, TextureDesc parameters, TextureLoadOptions loadoptions)
        {
            return new VKTexture2D(name, filename, parameters, loadoptions);
        }
    }
}