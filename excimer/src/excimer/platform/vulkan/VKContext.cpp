#include "hzpch.h"
#include "VKContext.h"
#include "excimer/core/Profiler.h"

#define VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME "VK_LAYER_LUNARG_standard_validation"
#define VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME "VK_LAYER_LUNARG_assistant_layer"
#define VK_LAYER_LUNARG_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace Excimer
{
	namespace Graphics
	{

		VkInstance VKContext::s_VkInstance = nullptr;

		const std::vector<const char*> VKContext::GetRequiredLayers() const
		{
			std::vector<const char*> layers;

			if (m_StandardValidationLayer)
				layers.emplace_back(VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME);

			if (m_AssistanceLayer)
				layers.emplace_back(VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME);

			if (EnableValidationLayers)
			{
				layers.emplace_back(VK_LAYER_LUNARG_VALIDATION_NAME);
			}

			return layers;
		}

		const std::vector<const char*> VKContext::GetRequiredExtensions()
		{
			std::vector<const char*> extensions;

			if (EnableValidationLayers)
			{
				EXCIMER_LOG_INFO("Vulkan : Enabled Validation Layers");
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
			}

			extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
			extensions.push_back("VK_KHR_portability_enumeration");


			extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

			return extensions;
		}

		VKContext::VKContext()
		{

		}

		void VKContext::MakeDefault()
		{
			CreateFunc = CreateFuncVulkan;
		}

		

		GraphicsContext* VKContext::CreateFuncVulkan()
		{
			return new VKContext();
		}

		void VKContext::Init()
		{
			EXCIMER_PROFILE_FUNCTION();
			CreateInstance();


		}

		bool VKContext::CheckValidationLayerSupport(std::vector<const char*>& validationLayers)
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);


		}

		void VKContext::CreateInstance()
		{
			EXCIMER_PROFILE_FUNCTION();
			if (volkInitialize() != VK_SUCCESS)
			{
				EXCIMER_LOG_CRITICAL("volkInitialize failed");
			}

			if (volkGetInstanceVersion() == 0)
			{
				EXCIMER_LOG_CRITICAL("Could not find loader");
			}

			m_InstanceLayerNames = GetRequiredLayers();
			m_InstanceExtensionNames = GetRequiredExtensions();

			if (!CheckValidationLayerSupport(m_InstanceLayerNames))
			{
				EXCIMER_LOG_CRITICAL("[VULKAN] Validation layers requested, but not available!");
			}

			if (!CheckExtensionSupport(m_InstanceExtensionNames))
			{
				EXCIMER_LOG_CRITICAL("[VULKAN] Extensions requested are not available!");
			}
		}
	}
}