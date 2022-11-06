#pragma once
#include "VK.h"

#include "excimer/graphics/rhi/Defintions.h"
#include "excimer/core/ExLog.h"
#include <vulkan/vk_mem_alloc.h>

#define VK_CHECK_RESULT(f)                                                                                                                        \
    {                                                                                                                                             \
        VkResult res = (f);                                                                                                                       \
        if(res != VK_SUCCESS)                                                                                                                     \
        {                                                                                                                                         \
            EXCIMER_LOG_ERROR("[VULKAN] : VkResult is {0} in {1} at line {2}", Excimer::Graphics::VKUtilities::ErrorString(res), __FILE__, __LINE__); \
        }                                                                                                                                         \
    }

namespace Excimer
{
    namespace Graphics
    {
        namespace VKUtilities
        {
            bool IsPresentModeSupported(const std::vector<VkPresentModeKHR>& supportedModes, VkPresentModeKHR presentMode);
            VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& supportedModes, bool vsync);

            void WaitIdle();
            std::string ErrorString(VkResult errorCode);
        }
    }
}
