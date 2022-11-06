#include "hzpch.h"
#include "VKUtilities.h"
#include "excimer/core/Profiler.h"
#include "VKDevice.h"



namespace Excimer
{
	namespace Graphics
	{
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
	}
}