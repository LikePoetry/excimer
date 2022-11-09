#include "hzpch.h"
#include "VKTexture.h"
#include "VKDevice.h"
#include "excimer/utilities/LoadImage.h"
#include "VKUtilities.h"
#include "VKBuffer.h"
#include "VKRenderer.h"
#include "excimer/core/Profiler.h"
#include "excimer/maths/Maths.h"

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
            : m_TextureImage(image)
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
            VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

            if (m_TextureSampler)
            {
                auto sampler = m_TextureSampler;
                deletionQueue.PushFunction([sampler]
                    { vkDestroySampler(VKDevice::GetHandle(), sampler, nullptr); });
            }

            if (m_TextureImageView)
            {
                auto imageView = m_TextureImageView;
                deletionQueue.PushFunction([imageView]
                    { vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr); });
            }

            for (auto& view : m_MipImageViews)
            {
                if (view.second)
                {
                    auto imageView = view.second;
                    deletionQueue.PushFunction([imageView]
                        { vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr); });
                }
            }

            m_MipImageViews.clear();

            if (m_DeleteImage)
            {
                auto image = m_TextureImage;
                auto alloc = m_Allocation;
                deletionQueue.PushFunction([image, alloc]
                    { vmaDestroyImage(VKDevice::Get().GetAllocator(), image, alloc); });

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

        void VKTexture2D::UpdateDescriptor()
        {
            m_Descriptor.sampler = m_TextureSampler;
            m_Descriptor.imageView = m_TextureImageView;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t layer = 0, uint32_t layerCount = 1)
        {
            EXCIMER_PROFILE_FUNCTION();
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(VKDevice::Get().GetGPU(), imageFormat, &formatProperties);

            if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
            {
                EXCIMER_LOG_ERROR("Texture image format does not support linear blitting!");
            }

            VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.image = image;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseArrayLayer = layer;
            barrier.subresourceRange.layerCount = layerCount;
            barrier.subresourceRange.levelCount = 1;

            int32_t mipWidth = texWidth;
            int32_t mipHeight = texHeight;

            for (uint32_t i = 1; i < mipLevels; i++)
            {
                barrier.subresourceRange.baseMipLevel = i - 1;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    0,
                    0,
                    nullptr,
                    0,
                    nullptr,
                    1,
                    &barrier);

                VkImageBlit blit{};
                blit.srcOffsets[0] = { 0, 0, 0 };
                blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
                blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.srcSubresource.mipLevel = i - 1;
                blit.srcSubresource.baseArrayLayer = layer;
                blit.srcSubresource.layerCount = layerCount;
                blit.dstOffsets[0] = { 0, 0, 0 };
                blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
                blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                blit.dstSubresource.mipLevel = i;
                blit.dstSubresource.baseArrayLayer = layer;
                blit.dstSubresource.layerCount = layerCount;

                vkCmdBlitImage(commandBuffer,
                    image,
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    1,
                    &blit,
                    VK_FILTER_LINEAR);

                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                vkCmdPipelineBarrier(commandBuffer,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    0,
                    0,
                    nullptr,
                    0,
                    nullptr,
                    1,
                    &barrier);

                if (mipWidth > 1)
                    mipWidth /= 2;
                if (mipHeight > 1)
                    mipHeight /= 2;
            }

            barrier.subresourceRange.baseMipLevel = mipLevels - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier);

            VKUtilities::EndSingleTimeCommands(commandBuffer);
        }

        bool VKTexture2D::Load()
        {
            EXCIMER_PROFILE_FUNCTION();
            uint32_t bits;
            uint8_t* pixels;

            m_Flags |= TextureFlags::Texture_Sampled;

            if (m_Data == nullptr)
            {
                pixels = Excimer::LoadImageFromFile(m_FileName, &m_Width, &m_Height, &bits);
                if (pixels == nullptr)
                    return false;

                m_Parameters.format = BitsToFormat(bits);
                m_Format = m_Parameters.format;
            }
            else
            {
                if (m_Data == nullptr)
                    return false;

                m_BitsPerChannel = GetBitsFromFormat(m_Parameters.format);
                bits = m_BitsPerChannel;
                pixels = m_Data;
            }

            m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);

            VkDeviceSize imageSize = VkDeviceSize(m_Width * m_Height * bits / 8);

            if (!pixels)
            {
                EXCIMER_LOG_CRITICAL("failed to load texture image!");
            }

            m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(Maths::Max(m_Width, m_Height)))) + 1;

            if (!(m_Flags & TextureFlags::Texture_CreateMips))
                m_MipLevels = 1;

            VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<uint32_t>(imageSize), pixels);

            if (m_Data == nullptr)
                delete[] pixels;

            Graphics::CreateImage(m_Width, m_Height, m_MipLevels, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0, m_Allocation);


            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
            VKUtilities::CopyBufferToImage(stagingBuffer->GetBuffer(), m_TextureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));

            delete stagingBuffer;

            if (m_Flags & TextureFlags::Texture_CreateMips)
                GenerateMipmaps(m_TextureImage, m_VKFormat, m_Width, m_Height, m_MipLevels);

            TransitionImage(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            return true;
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

        void VKTexture2D::TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer)
        {
            EXCIMER_PROFILE_FUNCTION();
            if (newLayout != m_ImageLayout)
                VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, m_ImageLayout, newLayout, m_MipLevels, 1, commandBuffer ? commandBuffer->GetHandle() : nullptr);
            m_ImageLayout = newLayout;
            UpdateDescriptor();
        }

        VkImageView VKTexture2D::GetMipImageView(uint32_t mip)
        {
            if (m_MipImageViews.find(mip) == m_MipImageViews.end())
            {
                m_MipImageViews[mip] = CreateImageView(m_TextureImage, m_VKFormat, 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, mip);
            }
            return m_MipImageViews.at(mip);
        }

        VKTextureCube::VKTextureCube(uint32_t size, void* data, bool hdr)
            : m_ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
        {
            m_Parameters = TextureDesc();
            m_Format = hdr ? RHIFormat::R32G32B32A32_Float : RHIFormat::R8G8B8A8_Unorm;
            m_Parameters.format = m_Format;
            m_VKFormat = VKUtilities::FormatToVK(m_Format, m_Parameters.srgb);

            m_Width = size;
            m_Height = size;
            m_Data = (uint8_t*)data;
            m_NumMips = static_cast<uint32_t>(std::floor(std::log2(Maths::Max(m_Width, m_Height)))) + 1;
            m_NumLayers = 6;
            m_DeleteImage = true;

            m_Flags |= TextureFlags::Texture_Sampled;

            m_BitsPerChannel = hdr ? 32 : 8;
            m_ChannelCount = 4;

            uint32_t dataSize = m_Width * m_Height * GetBytesPerPixel() * m_NumLayers;
            uint8_t* allData = new uint8_t[dataSize];
            uint32_t pointeroffset = 0;

            uint32_t faceOrder[6] = { 3, 1, 0, 4, 2, 5 };

            Graphics::CreateImage(m_Width, m_Height, m_NumMips, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, m_NumLayers, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, m_Allocation);


            m_TextureSampler = Graphics::CreateTextureSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, static_cast<float>(m_NumMips), true, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT);
            m_TextureImageView = Graphics::CreateImageView(m_TextureImage, m_VKFormat, m_NumMips, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, m_NumLayers);

            if (m_Data)
            {
                memcpy(allData, m_Data, dataSize);

                VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<uint32_t>(dataSize), allData);

                if (m_Data == nullptr)
                {
                    delete[] allData;
                    allData = nullptr;
                }

                VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommands();

                //// Setup buffer copy regions for each face including all of it's miplevels
                std::vector<VkBufferImageCopy> bufferCopyRegions;
                uint32_t offset = 0;

                for (uint32_t face = 0; face < m_NumLayers; face++)
                {
                    for (uint32_t level = 0; level < m_NumMips; level++)
                    {
                        VkBufferImageCopy bufferCopyRegion = {};
                        bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        bufferCopyRegion.imageSubresource.mipLevel = level;
                        bufferCopyRegion.imageSubresource.baseArrayLayer = face;
                        bufferCopyRegion.imageSubresource.layerCount = 1;
                        bufferCopyRegion.imageExtent.width = m_Width;
                        bufferCopyRegion.imageExtent.height = m_Height;
                        bufferCopyRegion.imageExtent.depth = 1;
                        bufferCopyRegion.bufferOffset = offset;

                        bufferCopyRegions.push_back(bufferCopyRegion);

                        // Increase offset into staging buffer for next level / face
                        offset += m_Width * m_Height * GetBytesPerPixel();
                    }
                }

                // Image barrier for optimal image (target)
                // Set initial layout for all array layers (faces) of the optimal (target) tiled texture
                VkImageSubresourceRange subresourceRange = {};
                subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                subresourceRange.baseMipLevel = 0;
                subresourceRange.levelCount = m_NumMips;
                subresourceRange.layerCount = m_NumLayers;

                VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_NumMips, m_NumLayers, commandBuffer);

                // Copy the cube map faces from the staging buffer to the optimal tiled image
                vkCmdCopyBufferToImage(
                    commandBuffer,
                    stagingBuffer->GetBuffer(),
                    m_TextureImage,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    static_cast<uint32_t>(bufferCopyRegions.size()),
                    bufferCopyRegions.data());

                // Change texture image layout to shader read after all faces have been copied
                VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_NumMips, m_NumLayers, commandBuffer);

                m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                VKUtilities::EndSingleTimeCommands(commandBuffer);

                delete stagingBuffer;
            }
            delete[] allData;

            for (uint32_t i = 0; i < m_NumLayers; i++)
            {
                VkImageView imageView = CreateImageView(m_TextureImage, m_VKFormat, 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, i, 0);
                m_IndividualImageViews.push_back(imageView);
            }

            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, m_NumMips, m_NumLayers, nullptr);

            m_ImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            UpdateDescriptor();
        }

        VKTextureCube::VKTextureCube(const std::string* files)
        {
            for (uint32_t i = 0; i < 6; i++)
                m_Files[i] = files[i];

            Load(1);

            UpdateDescriptor();
        }

        VKTextureCube::VKTextureCube(const std::string* files, uint32_t mips, TextureDesc params, TextureLoadOptions loadOptions)
        {
            m_Parameters = params;
            m_NumMips = mips;
            for (uint32_t i = 0; i < mips; i++)
                m_Files[i] = files[i];

            Load(mips);
            m_Format = m_Parameters.format;

            UpdateDescriptor();
        }

        VKTextureCube::VKTextureCube(const std::string& filepath)
            : m_Width(0)
            , m_Height(0)
            , m_Size(0)
            , m_NumMips(0)
            , m_ImageLayout()
        {
            m_Files[0] = filepath;
            m_Format = m_Parameters.format;
        }

        VKTextureCube::~VKTextureCube()
        {
            VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

            if (m_TextureSampler)
            {
                auto sampler = m_TextureSampler;
                deletionQueue.PushFunction([sampler]
                    { vkDestroySampler(VKDevice::GetHandle(), sampler, nullptr); });
            }

            if (m_TextureImageView)
            {
                auto imageView = m_TextureImageView;
                deletionQueue.PushFunction([imageView]

                    { vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr); });
            }

            if (!m_IndividualImageViews.empty())
            {
                auto imageViews = m_IndividualImageViews;
                deletionQueue.PushFunction([imageViews]

                    {
                        for (uint32_t i = 0; i < (uint32_t)imageViews.size(); i++)
                        {
                            vkDestroyImageView(VKDevice::GetHandle(), imageViews[i], nullptr);
                        } });
            }

            if (m_DeleteImage)
            {
                auto image = m_TextureImage;
                auto alloc = m_Allocation;
                deletionQueue.PushFunction([image, alloc]
                    { vmaDestroyImage(VKDevice::Get().GetAllocator(), image, alloc); });

            }
        }

        void VKTextureCube::TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer)
        {
            EXCIMER_PROFILE_FUNCTION();
            if (newLayout != m_ImageLayout)
                VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, m_ImageLayout, newLayout, m_NumMips, 6, commandBuffer ? commandBuffer->GetHandle() : nullptr);
            m_ImageLayout = newLayout;
            UpdateDescriptor();
        }

        void VKTextureCube::GenerateMipMaps()
        {
            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, m_ImageLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_NumMips, 6);

            for (int i = 0; i < 6; i++)
                GenerateMipmaps(m_TextureImage, m_VKFormat, m_Width, m_Height, m_NumMips, i, 1);

            // Generate mips sets layout to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_ImageLayout, m_NumMips, 6);
        }

        void VKTextureCube::UpdateDescriptor()
        {
            m_Descriptor.sampler = m_TextureSampler;
            m_Descriptor.imageView = m_TextureImageView;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        void VKTextureCube::Load(uint32_t mips)
        {
            EXCIMER_PROFILE_FUNCTION();
            uint32_t srcWidth, srcHeight, bits = 0;
            uint8_t*** cubeTextureData = new uint8_t * *[mips];
            for (uint32_t i = 0; i < mips; i++)
                cubeTextureData[i] = new uint8_t * [6];

            uint32_t* faceWidths = new uint32_t[mips];
            uint32_t* faceHeights = new uint32_t[mips];
            uint32_t size = 0;
            bool isHDR = false;

            m_Flags |= TextureFlags::Texture_Sampled;

            for (uint32_t m = 0; m < mips; m++)
            {
                uint8_t* data = Excimer::LoadImageFromFile(m_Files[m], &srcWidth, &srcHeight, &bits, &isHDR, !m_LoadOptions.flipY);
                m_Parameters.format = BitsToFormat(bits);

                uint32_t face = 0;
                uint32_t faceWidth = srcWidth / 3;
                uint32_t faceHeight = srcHeight / 4;

                if (m == 0)
                {
                    m_Width = faceWidth;
                    m_Height = faceHeight;
                    m_BitsPerChannel = bits;
                    m_ChannelCount = BitsToChannelCount(bits);
                    m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);
                }

                uint32_t stride = GetBytesPerChannel();

                faceWidths[m] = faceWidth;
                faceHeights[m] = faceHeight;
                for (uint32_t cy = 0; cy < 4; cy++)
                {
                    for (uint32_t cx = 0; cx < 3; cx++)
                    {
                        if (cy == 0 || cy == 2 || cy == 3)
                        {
                            if (cx != 1)
                                continue;
                        }

                        cubeTextureData[m][face] = new uint8_t[faceWidth * faceHeight * stride];

                        size += stride * srcHeight * srcWidth;

                        for (uint32_t y = 0; y < faceHeight; y++)
                        {
                            uint32_t offset = y;
                            if (face == 5)
                                offset = faceHeight - (y + 1);
                            uint32_t yp = cy * faceHeight + offset;
                            for (uint32_t x = 0; x < faceWidth; x++)
                            {
                                offset = x;
                                if (face == 5)
                                    offset = faceWidth - (x + 1);
                                uint32_t xp = cx * faceWidth + offset;
                                cubeTextureData[m][face][(x + y * faceWidth) * stride + 0] = data[(xp + yp * srcWidth) * stride + 0];
                                cubeTextureData[m][face][(x + y * faceWidth) * stride + 1] = data[(xp + yp * srcWidth) * stride + 1];
                                cubeTextureData[m][face][(x + y * faceWidth) * stride + 2] = data[(xp + yp * srcWidth) * stride + 2];
                                if (stride >= 4)
                                    cubeTextureData[m][face][(x + y * faceWidth) * stride + 3] = data[(xp + yp * srcWidth) * stride + 3];
                            }
                        }
                        face++;
                    }
                }
                delete[] data;
            }

            uint8_t* allData = new uint8_t[size];
            uint32_t pointeroffset = 0;

            uint32_t faceOrder[6] = { 3, 1, 0, 4, 2, 5 };

            for (uint32_t face = 0; face < 6; face++)
            {
                for (uint32_t mip = 0; mip < m_NumMips; mip++)
                {
                    uint32_t currentSize = faceWidths[mip] * faceHeights[mip] * GetBytesPerChannel();
                    memcpy(allData + pointeroffset, cubeTextureData[mip][faceOrder[face]], currentSize);
                    pointeroffset += currentSize;
                }
            }

            VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<uint32_t>(size), allData);

            if (m_Data == nullptr)
            {
                delete[] allData;
                allData = nullptr;
            }
            Graphics::CreateImage(faceWidths[0], faceHeights[0], m_NumMips, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, m_Allocation);


            VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommands();

            //// Setup buffer copy regions for each face including all of it's miplevels
            std::vector<VkBufferImageCopy> bufferCopyRegions;
            uint32_t offset = 0;

            for (uint32_t face = 0; face < 6; face++)
            {
                for (uint32_t level = 0; level < m_NumMips; level++)
                {
                    VkBufferImageCopy bufferCopyRegion = {};
                    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    bufferCopyRegion.imageSubresource.mipLevel = level;
                    bufferCopyRegion.imageSubresource.baseArrayLayer = face;
                    bufferCopyRegion.imageSubresource.layerCount = 1;
                    bufferCopyRegion.imageExtent.width = faceWidths[level];
                    bufferCopyRegion.imageExtent.height = faceHeights[level];
                    bufferCopyRegion.imageExtent.depth = 1;
                    bufferCopyRegion.bufferOffset = offset;

                    bufferCopyRegions.push_back(bufferCopyRegion);

                    // Increase offset into staging buffer for next level / face
                    offset += faceWidths[level] * faceWidths[level] * GetBytesPerChannel();
                }
            }

            // Image barrier for optimal image (target)
            // Set initial layout for all array layers (faces) of the optimal (target) tiled texture
            VkImageSubresourceRange subresourceRange = {};
            subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresourceRange.baseMipLevel = 0;
            subresourceRange.levelCount = m_NumMips;
            subresourceRange.layerCount = 6;

            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_NumMips, 6, commandBuffer);

            // Copy the cube map faces from the staging buffer to the optimal tiled image
            vkCmdCopyBufferToImage(
                commandBuffer,
                stagingBuffer->GetBuffer(),
                m_TextureImage,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                static_cast<uint32_t>(bufferCopyRegions.size()),
                bufferCopyRegions.data());

            // Change texture image layout to shader read after all faces have been copied
            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, m_NumMips, 6, commandBuffer);

            m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VKUtilities::EndSingleTimeCommands(commandBuffer);

            m_TextureSampler = Graphics::CreateTextureSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, static_cast<float>(m_NumMips), true, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT);
            m_TextureImageView = Graphics::CreateImageView(m_TextureImage, m_VKFormat, m_NumMips, VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_ASPECT_COLOR_BIT, 6);

            delete stagingBuffer;

            for (uint32_t m = 0; m < mips; m++)
            {
                for (uint32_t f = 0; f < 6; f++)
                {
                    delete[] cubeTextureData[m][f];
                }
                delete[] cubeTextureData[m];
            }
            delete[] cubeTextureData;
            delete[] faceHeights;
            delete[] faceWidths;
            delete[] allData;

            for (uint32_t i = 0; i < 6; i++)
            {
                VkImageView imageView = CreateImageView(m_TextureImage, VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb), 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, i);

                m_IndividualImageViews.push_back(imageView);
            }
        }

        VKTextureDepth::VKTextureDepth(uint32_t width, uint32_t height)
            : m_Width(width)
            , m_Height(height)
            , m_TextureImageView(VK_NULL_HANDLE)
            , m_TextureSampler(VK_NULL_HANDLE)
        {
            Init();
        }

        VKTextureDepth::~VKTextureDepth()
        {
            VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

            auto sampler = m_TextureSampler;
            auto imageView = m_TextureImageView;

            deletionQueue.PushFunction([sampler, imageView]
                {
                    vkDestroySampler(VKDevice::GetHandle(), sampler, nullptr);
                    vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr); });

            auto textureImage = m_TextureImage;
            auto alloc = m_Allocation;

            deletionQueue.PushFunction([textureImage, alloc]
                { vmaDestroyImage(VKDevice::Get().GetAllocator(), textureImage, alloc); });

        }

        void VKTextureDepth::Init()
        {
            EXCIMER_PROFILE_FUNCTION();
            VkFormat depthFormat = VKUtilities::FindDepthFormat();
            m_VKFormat = depthFormat;
            m_Format = VKUtilities::VKToFormat(depthFormat);

            Graphics::CreateImage(m_Width, m_Height, 1, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0, m_Allocation);

            m_TextureImageView = CreateImageView(m_TextureImage, m_VKFormat, 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

            m_ImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            m_TextureSampler = Graphics::CreateTextureSampler(VK_FILTER_LINEAR, VK_FILTER_LINEAR, 0.0f, 1.0f, false, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);

            m_Flags |= TextureFlags::Texture_DepthStencil;

            UpdateDescriptor();
        }

        void VKTextureDepth::UpdateDescriptor()
        {
            m_Descriptor.sampler = m_TextureSampler;
            m_Descriptor.imageView = m_TextureImageView;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        void VKTextureDepth::TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer)
        {
            EXCIMER_PROFILE_FUNCTION();
            if (newLayout != m_ImageLayout)
            {
                VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, m_ImageLayout, newLayout, 1, 1, commandBuffer ? commandBuffer->GetHandle() : nullptr);
            }

            m_ImageLayout = newLayout;
            UpdateDescriptor();
        }

        void VKTextureDepth::Resize(uint32_t width, uint32_t height)
        {
            EXCIMER_PROFILE_FUNCTION();

            m_Width = width;
            m_Height = height;

            VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

            auto sampler = m_TextureSampler;
            auto imageView = m_TextureImageView;

            deletionQueue.PushFunction([sampler, imageView]
                {
                    vkDestroySampler(VKDevice::GetHandle(), sampler, nullptr);
                    vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr); });

            auto textureImage = m_TextureImage;
            auto alloc = m_Allocation;

            deletionQueue.PushFunction([textureImage, alloc]
                { vmaDestroyImage(VKDevice::Get().GetAllocator(), textureImage, alloc); });
            m_TextureImage = VkImage();

            Init();
        }

        VKTextureDepthArray::VKTextureDepthArray(uint32_t width, uint32_t height, uint32_t count)
            : m_Width(width)
            , m_Height(height)
            , m_Count(count)
        {
            VKTextureDepthArray::Init();
        }

        VKTextureDepthArray::~VKTextureDepthArray()
        {
            EXCIMER_PROFILE_FUNCTION();
            VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

            auto sampler = m_TextureSampler;
            auto imageView = m_TextureImageView;
            auto imageViews = m_IndividualImageViews;

            deletionQueue.PushFunction([sampler, imageView, imageViews]
                {
                    vkDestroySampler(VKDevice::GetHandle(), sampler, nullptr);
                    vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr);

                    for (uint32_t i = 0; i < (uint32_t)imageViews.size(); i++)
                    {
                        vkDestroyImageView(VKDevice::GetHandle(), imageViews[i], nullptr);
                    } });

            auto textureImage = m_TextureImage;
            auto alloc = m_Allocation;

            deletionQueue.PushFunction([textureImage, alloc]
                { vmaDestroyImage(VKDevice::Get().GetAllocator(), textureImage, alloc); });

        }

        void VKTextureDepthArray::Init()
        {
            EXCIMER_PROFILE_FUNCTION();
            m_Flags |= TextureFlags::Texture_DepthStencil;

            VkFormat depthFormat = VKUtilities::FindDepthFormat();
            m_VKFormat = VK_FORMAT_D32_SFLOAT; // depthFormat;
            m_Format = VKUtilities::VKToFormat(m_VKFormat);

            Graphics::CreateImage(m_Width, m_Height, 1, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, m_Count, 0, m_Allocation);

            m_TextureImageView = CreateImageView(m_TextureImage, m_VKFormat, 1, VK_IMAGE_VIEW_TYPE_2D_ARRAY, VK_IMAGE_ASPECT_DEPTH_BIT, m_Count);

            for (uint32_t i = 0; i < m_Count; i++)
            {
                VkImageView imageView = CreateImageView(m_TextureImage, m_VKFormat, 1, VK_IMAGE_VIEW_TYPE_2D_ARRAY, VK_IMAGE_ASPECT_DEPTH_BIT, 1, i);
                m_IndividualImageViews.push_back(imageView);
            }

            VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1, m_Count);
            m_ImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            m_TextureSampler = Graphics::CreateTextureSampler();

            UpdateDescriptor();
        }

        void VKTextureDepthArray::UpdateDescriptor()
        {
            m_Descriptor.sampler = m_TextureSampler;
            m_Descriptor.imageView = m_TextureImageView;
            m_Descriptor.imageLayout = m_ImageLayout;
        }

        void* VKTextureDepthArray::GetHandleArray(uint32_t index)
        {
            m_Descriptor.imageView = GetImageView(index);
            return (void*)&m_Descriptor;
        }

        void VKTextureDepthArray::TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer)
        {
            EXCIMER_PROFILE_FUNCTION();
            if (newLayout != m_ImageLayout)
                VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, m_ImageLayout, newLayout, 1, m_Count, commandBuffer ? commandBuffer->GetHandle() : nullptr);
            m_ImageLayout = newLayout;
            UpdateDescriptor();
        }

        void VKTextureDepthArray::Resize(uint32_t width, uint32_t height, uint32_t count)
        {
            EXCIMER_PROFILE_FUNCTION();
            m_Width = width;
            m_Height = height;
            m_Count = count;

            VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

            auto sampler = m_TextureSampler;
            auto imageView = m_TextureImageView;
            auto imageViews = m_IndividualImageViews;

            deletionQueue.PushFunction([sampler, imageView, imageViews]
                {
                    vkDestroySampler(VKDevice::GetHandle(), sampler, nullptr);
                    vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr);

                    for (uint32_t i = 0; i < (uint32_t)imageViews.size(); i++)
                    {
                        vkDestroyImageView(VKDevice::GetHandle(), imageViews[i], nullptr);
                    } });

            auto textureImage = m_TextureImage;
            auto alloc = m_Allocation;

            deletionQueue.PushFunction([textureImage, alloc]
                { vmaDestroyImage(VKDevice::Get().GetAllocator(), textureImage, alloc); });


            m_TextureImage = VkImage();

            Init();
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

        TextureCube* VKTextureCube::CreateFuncVulkan(uint32_t size, void* data, bool hdr)
        {
            return new VKTextureCube(size, data, hdr);
        }

        TextureCube* VKTextureCube::CreateFromFileFuncVulkan(const std::string& filepath)
        {
            return new VKTextureCube(filepath);
        }

        TextureCube* VKTextureCube::CreateFromFilesFuncVulkan(const std::string* files)
        {
            return new VKTextureCube(files);
        }

        TextureCube* VKTextureCube::CreateFromVCrossFuncVulkan(const std::string* files, uint32_t mips, TextureDesc params, TextureLoadOptions loadOptions)
        {
            return new VKTextureCube(files, mips, params, loadOptions);
        }

        TextureDepth* VKTextureDepth::CreateFuncVulkan(uint32_t width, uint32_t height)
        {
            return new VKTextureDepth(width, height);
        }

        TextureDepthArray* VKTextureDepthArray::CreateFuncVulkan(uint32_t width, uint32_t height, uint32_t count)
        {
            return new VKTextureDepthArray(width, height, count);
        }

        void VKTexture2D::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
            CreateFromFileFunc = CreateFromFileFuncVulkan;
            CreateFromSourceFunc = CreateFromSourceFuncVulkan;
        }

        void VKTextureCube::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
            CreateFromFileFunc = CreateFromFileFuncVulkan;
            CreateFromFilesFunc = CreateFromFilesFuncVulkan;
            CreateFromVCrossFunc = CreateFromVCrossFuncVulkan;
        }

        void VKTextureDepth::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
        }

        void VKTextureDepthArray::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
        }
    }
}
