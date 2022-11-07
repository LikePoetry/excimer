#include "hzpch.h"
#include "VKUtilities.h"
#include "excimer/core/Profiler.h"
#include "excimer/core/ExLog.h"
#include "VKDevice.h"
#include "VKInitialisers.h"




namespace Excimer
{
	namespace Graphics
	{
		VkPipelineStageFlags AccessFlagsToPipelineStage(VkAccessFlags accessFlags, const VkPipelineStageFlags stageFlags)
		{
			VkPipelineStageFlags stages = 0;

			while (accessFlags != 0)
			{
				VkAccessFlagBits AccessFlag = static_cast<VkAccessFlagBits>(accessFlags & (~(accessFlags - 1)));
				EXCIMER_ASSERT(AccessFlag != 0 && (AccessFlag & (AccessFlag - 1)) == 0, "Error");
				accessFlags &= ~AccessFlag;

				switch (AccessFlag)
				{
				case VK_ACCESS_INDIRECT_COMMAND_READ_BIT:
					stages |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
					break;

				case VK_ACCESS_INDEX_READ_BIT:
					stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
					break;

				case VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT:
					stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
					break;

				case VK_ACCESS_UNIFORM_READ_BIT:
					stages |= stageFlags | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
					break;

				case VK_ACCESS_INPUT_ATTACHMENT_READ_BIT:
					stages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
					break;

				case VK_ACCESS_SHADER_READ_BIT:
					stages |= stageFlags | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
					break;

				case VK_ACCESS_SHADER_WRITE_BIT:
					stages |= stageFlags | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
					break;

				case VK_ACCESS_COLOR_ATTACHMENT_READ_BIT:
					stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
					break;

				case VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT:
					stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
					break;

				case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT:
					stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
					break;

				case VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT:
					stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
					break;

				case VK_ACCESS_TRANSFER_READ_BIT:
					stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
					break;

				case VK_ACCESS_TRANSFER_WRITE_BIT:
					stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
					break;

				case VK_ACCESS_HOST_READ_BIT:
					stages |= VK_PIPELINE_STAGE_HOST_BIT;
					break;

				case VK_ACCESS_HOST_WRITE_BIT:
					stages |= VK_PIPELINE_STAGE_HOST_BIT;
					break;

				case VK_ACCESS_MEMORY_READ_BIT:
					break;

				case VK_ACCESS_MEMORY_WRITE_BIT:
					break;

				default:
					EXCIMER_LOG_ERROR("Unknown access flag");
					break;
				}
			}
			return stages;
		}

		VkPipelineStageFlags LayoutToAccessMask(const VkImageLayout layout, const bool isDestination)
		{
			VkPipelineStageFlags accessMask = 0;

			switch (layout)
			{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				if (isDestination)
				{
					EXCIMER_LOG_ERROR("The new layout used in a transition must not be VK_IMAGE_LAYOUT_UNDEFINED.");
				}
				break;

			case VK_IMAGE_LAYOUT_GENERAL:
				accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				accessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
				accessMask = VK_ACCESS_SHADER_READ_BIT; // VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				accessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_PREINITIALIZED:
				if (!isDestination)
				{
					accessMask = VK_ACCESS_HOST_WRITE_BIT;
				}
				else
				{
					EXCIMER_LOG_ERROR("The new layout used in a transition must not be VK_IMAGE_LAYOUT_PREINITIALIZED.");
				}
				break;

			case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
				accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
				accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				accessMask = VK_ACCESS_MEMORY_READ_BIT;
				break;

			default:
				EXCIMER_LOG_ERROR("Unexpected image layout");
				break;
			}

			return accessMask;
		}

		bool IsDepthFormat(VkFormat format)
		{
			switch (format)
			{
			case VK_FORMAT_D16_UNORM:
			case VK_FORMAT_D32_SFLOAT:
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
			case VK_FORMAT_D24_UNORM_S8_UINT:
			case VK_FORMAT_D16_UNORM_S8_UINT:
				return true;
			}
			return false;
		}

		bool IsStencilFormat(VkFormat format)
		{
			switch (format)
			{
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
			case VK_FORMAT_D24_UNORM_S8_UINT:
				return true;
			}
			return false;
		}

		VkCommandBuffer VKUtilities::BeginSingleTimeCommands()
		{
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = VKDevice::Get().GetCommandPool()->GetHandle();
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(VKDevice::Get().GetDevice(), &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));

			return commandBuffer;
		}

		void VKUtilities::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
		{
			VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

			VkSubmitInfo submitInfo;
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;
			submitInfo.pSignalSemaphores = nullptr;
			submitInfo.pNext = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
			submitInfo.signalSemaphoreCount = 0;
			submitInfo.waitSemaphoreCount = 0;

			VK_CHECK_RESULT(vkQueueSubmit(VKDevice::Get().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));
			VK_CHECK_RESULT(vkQueueWaitIdle(VKDevice::Get().GetGraphicsQueue()));

			vkFreeCommandBuffers(VKDevice::Get().GetDevice(),
				VKDevice::Get().GetCommandPool()->GetHandle(), 1, &commandBuffer);
		}

		void VKUtilities::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldImageLayout, VkImageLayout newImageLayout,
			uint32_t mipLevels, uint32_t layerCount, VkCommandBuffer commandBuffer)
		{
			EXCIMER_PROFILE_FUNCTION();

			bool singleTimeCommand = false;

			if (!commandBuffer)
			{
				commandBuffer = BeginSingleTimeCommands();
				singleTimeCommand = true;
			}

			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = IsDepthFormat(format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

			if (IsStencilFormat(format))
				subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = mipLevels;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = layerCount;

			// Create an image barrier object
			VkImageMemoryBarrier imageMemoryBarrier = VKInitialisers::ImageMemoryBarrier();
			imageMemoryBarrier.oldLayout = oldImageLayout;
			imageMemoryBarrier.newLayout = newImageLayout;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = subresourceRange;
			imageMemoryBarrier.srcAccessMask = LayoutToAccessMask(oldImageLayout, false);
			imageMemoryBarrier.dstAccessMask = LayoutToAccessMask(newImageLayout, true);
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			VkPipelineStageFlags sourceStage = 0;
			{
				if (imageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
				{
					sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				}
				else if (imageMemoryBarrier.srcAccessMask != 0)
				{
					sourceStage = AccessFlagsToPipelineStage(imageMemoryBarrier.srcAccessMask, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
				}
				else
				{
					sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				}
			}

			VkPipelineStageFlags destinationStage = 0;
			{
				if (imageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
				{
					destinationStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				}
				else if (imageMemoryBarrier.dstAccessMask != 0)
				{
					destinationStage = AccessFlagsToPipelineStage(imageMemoryBarrier.dstAccessMask, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
				}
				else
				{
					destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				}
			}

			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier(
				commandBuffer,
				sourceStage,
				destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);

			if (singleTimeCommand)
				EndSingleTimeCommands(commandBuffer);
		}

		bool VKUtilities::IsPresentModeSupported(const std::vector<VkPresentModeKHR>& supportedModes, VkPresentModeKHR presentMode)
		{
			for (const auto& mode : supportedModes)
			{
				if (mode == presentMode)
				{
					return true;
				}
			}
			return false;
		}

		VkPresentModeKHR VKUtilities::ChoosePresentMode(const std::vector<VkPresentModeKHR>& supportedModes, bool vsync)
		{
			VkPresentModeKHR presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			if (vsync)
			{
				if (IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_MAILBOX_KHR))
				{
					presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				}
				else if (IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_FIFO_KHR))
				{
					presentMode = VK_PRESENT_MODE_FIFO_KHR;
				}
				else if (IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
				else
				{
					EXCIMER_LOG_ERROR("Failed to find supported presentation mode.");
				}
			}
			else
			{
				if (IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
				else if (IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_FIFO_RELAXED_KHR))
				{
					presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
				}
				else if (IsPresentModeSupported(supportedModes, VK_PRESENT_MODE_FIFO_KHR))
				{
					presentMode = VK_PRESENT_MODE_FIFO_KHR;
				}
				else
				{
					EXCIMER_LOG_ERROR("Failed to find supported presentation mode.");
				}
			}

			return presentMode;
		}

		std::string VKUtilities::ErrorString(VkResult errorCode)
		{
			switch (errorCode)
			{
#define STR(r)   \
    case VK_##r: \
        return #r
				STR(NOT_READY);
				STR(TIMEOUT);
				STR(EVENT_SET);
				STR(EVENT_RESET);
				STR(INCOMPLETE);
				STR(ERROR_OUT_OF_HOST_MEMORY);
				STR(ERROR_OUT_OF_DEVICE_MEMORY);
				STR(ERROR_INITIALIZATION_FAILED);
				STR(ERROR_DEVICE_LOST);
				STR(ERROR_MEMORY_MAP_FAILED);
				STR(ERROR_LAYER_NOT_PRESENT);
				STR(ERROR_EXTENSION_NOT_PRESENT);
				STR(ERROR_FEATURE_NOT_PRESENT);
				STR(ERROR_INCOMPATIBLE_DRIVER);
				STR(ERROR_TOO_MANY_OBJECTS);
				STR(ERROR_FORMAT_NOT_SUPPORTED);
				STR(ERROR_SURFACE_LOST_KHR);
				STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
				STR(SUBOPTIMAL_KHR);
				STR(ERROR_OUT_OF_DATE_KHR);
				STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
				STR(ERROR_VALIDATION_FAILED_EXT);
				STR(ERROR_INVALID_SHADER_NV);
				STR(SUCCESS);
				STR(ERROR_FRAGMENTED_POOL);
				STR(ERROR_OUT_OF_POOL_MEMORY);
				STR(ERROR_INVALID_EXTERNAL_HANDLE);
				STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
				STR(ERROR_FRAGMENTATION_EXT);
				STR(ERROR_NOT_PERMITTED_EXT);
				STR(ERROR_INVALID_DEVICE_ADDRESS_EXT);
				STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
				STR(ERROR_UNKNOWN);
				STR(RESULT_MAX_ENUM);
#undef STR
			default:
				return "UNKNOWN_ERROR";
			}
		}

		void VKUtilities::WaitIdle()
		{
			EXCIMER_PROFILE_FUNCTION();
			vkDeviceWaitIdle(VKDevice::GetHandle());
		}

		VkFilter VKUtilities::TextureFilterToVK(const TextureFilter filter)
		{
			switch (filter)
			{
			case TextureFilter::NEAREST:
				return VK_FILTER_NEAREST;
			case TextureFilter::LINEAR:
				return VK_FILTER_LINEAR;
			case TextureFilter::NONE:
				return VK_FILTER_LINEAR;
			default:
				EXCIMER_LOG_CRITICAL("[Texture] Unsupported TextureFilter type!");
				return VK_FILTER_LINEAR;
			}
		}

		VkFormat VKUtilities::FormatToVK(const RHIFormat format, bool srgb)
		{
			if (srgb)
			{
				switch (format)
				{
				case RHIFormat::R8_Unorm:
					return VK_FORMAT_R8_SRGB;
				case RHIFormat::R8G8_Unorm:
					return VK_FORMAT_R8G8_SRGB;
				case RHIFormat::R8G8B8_Unorm:
					return VK_FORMAT_R8G8B8_SRGB;
				case RHIFormat::R8G8B8A8_Unorm:
					return VK_FORMAT_R8G8B8A8_SRGB;
				case RHIFormat::R16G16B16_Float:
					return VK_FORMAT_R16G16B16_SFLOAT;
				case RHIFormat::R16G16B16A16_Float:
					return VK_FORMAT_R16G16B16A16_SFLOAT;
				case RHIFormat::R32G32B32_Float:
					return VK_FORMAT_R32G32B32_SFLOAT;
				case RHIFormat::R32G32B32A32_Float:
					return VK_FORMAT_R32G32B32A32_SFLOAT;
				default:
					EXCIMER_LOG_CRITICAL("[Texture] Unsupported image bit-depth!");
					return VK_FORMAT_R8G8B8A8_SRGB;
				}
			}
			else
			{
				switch (format)
				{
				case RHIFormat::R8_Unorm:
					return VK_FORMAT_R8_UNORM;
				case RHIFormat::R8G8_Unorm:
					return VK_FORMAT_R8G8_UNORM;
				case RHIFormat::R8G8B8_Unorm:
					return VK_FORMAT_R8G8B8A8_UNORM;
				case RHIFormat::R8G8B8A8_Unorm:
					return VK_FORMAT_R8G8B8A8_UNORM;
				case RHIFormat::R11G11B10_Float:
					return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
				case RHIFormat::R10G10B10A2_Unorm:
					return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
				case RHIFormat::R16_Float:
					return VK_FORMAT_R16_SFLOAT;
				case RHIFormat::R16G16_Float:
					return VK_FORMAT_R16G16_SFLOAT;
				case RHIFormat::R16G16B16_Float:
					return VK_FORMAT_R16G16B16_SFLOAT;
				case RHIFormat::R16G16B16A16_Float:
					return VK_FORMAT_R16G16B16A16_SFLOAT;
				case RHIFormat::R32_Float:
					return VK_FORMAT_R32_SFLOAT;
				case RHIFormat::R32G32_Float:
					return VK_FORMAT_R32G32_SFLOAT;
				case RHIFormat::R32G32B32_Float:
					return VK_FORMAT_R32G32B32_SFLOAT;
				case RHIFormat::R32G32B32A32_Float:
					return VK_FORMAT_R32G32B32A32_SFLOAT;
				case RHIFormat::D16_Unorm:
					return VK_FORMAT_D16_UNORM;
				case RHIFormat::D32_Float:
					return VK_FORMAT_D32_SFLOAT;
				case RHIFormat::D24_Unorm_S8_UInt:
					return VK_FORMAT_D24_UNORM_S8_UINT;
				case RHIFormat::D32_Float_S8_UInt:
					return VK_FORMAT_D32_SFLOAT_S8_UINT;
				default:
					EXCIMER_LOG_CRITICAL("[Texture] Unsupported image bit-depth!");
					return VK_FORMAT_R8G8B8A8_UNORM;
				}
			}
		}

		VkSamplerAddressMode VKUtilities::TextureWrapToVK(const TextureWrap wrap)
		{
			switch (wrap)
			{
			case TextureWrap::CLAMP:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case TextureWrap::CLAMP_TO_BORDER:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			case TextureWrap::CLAMP_TO_EDGE:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case TextureWrap::REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case TextureWrap::MIRRORED_REPEAT:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			default:
				EXCIMER_LOG_CRITICAL("[Texture] Unsupported wrap type!");
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			}
		}
	}
}