#include "hzpch.h"
#include "VKContext.h"

#include "VKDevice.h"
#include "excimer/core/Version.h"
#include "excimer/maths/MathsUtilities.h"
#include "excimer/core/Profiler.h"
#include "excimer/core/StringUtilities.h"

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

		VKContext::~VKContext()
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
			VKDevice::Get().Init();

			SetupDebugCallback();
		}

		void VKContext::Present()
		{
		}

		void VKContext::Unload()
		{
		}

		VkBool32 VKContext::DebugCallback(VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t sourceObj,
			size_t location,
			int32_t msgCode,
			const char* pLayerPrefix,
			const char* pMsg,
			void* userData) 
		{
			if (!flags)
				return VK_FALSE;

			if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
			{
				EXCIMER_LOG_WARN("[VULKAN] - ERROR : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			};
			// Warnings may hint at unexpected / non-spec API usage
			if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
			{
				EXCIMER_LOG_WARN("[VULKAN] - WARNING : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			};
			// May indicate sub-optimal usage of the API
			if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
			{
				EXCIMER_LOG_WARN("[VULKAN] - PERFORMANCE : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			};
			// Informal messages that may become handy during debugging
			if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
			{
				EXCIMER_LOG_WARN("[VULKAN] - INFO : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			}
			// Diagnostic info from the Vulkan loader and layers
			// Usually not helpful in terms of API usage, but may help to debug layer and loader problems
			if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
			{
				EXCIMER_LOG_WARN("[VULKAN] - DEBUG : [{0}] Code {1}  : {2}", pLayerPrefix, msgCode, pMsg);
			}

			return VK_FALSE;
		}

		bool VKContext::CheckValidationLayerSupport(std::vector<const char*>& validationLayers)
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			m_InstanceLayers.resize(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, m_InstanceLayers.data());
			bool removedLayer = false;

			validationLayers.erase(
				std::remove_if(
					validationLayers.begin(),
					validationLayers.end(),
					[&](const char* layerName)
					{
						bool layerFound = false;

						for (const auto& layerProperties : m_InstanceLayers)
						{
							if (strcmp(layerName, layerProperties.layerName) == 0)
							{
								layerFound = true;
								break;
							}
						}

						if (!layerFound)
						{
							removedLayer = true;
							EXCIMER_LOG_WARN("[VULKAN] Layer not supported - {0}", layerName);
						}

						return !layerFound;
					}),
				validationLayers.end());

			return !removedLayer;

		}

		bool VKContext::CheckExtensionSupport(std::vector<const char*>& extensions)
		{
			uint32_t extensionCount;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			m_InstanceExtensions.resize(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_InstanceExtensions.data());

			bool removedExtension = false;

			extensions.erase(
				std::remove_if(
					extensions.begin(),
					extensions.end(),
					[&](const char* extensionName)
					{
						bool extensionFound = false;

						for (const auto& extensionProperties : m_InstanceExtensions)
						{
							if (strcmp(extensionName, extensionProperties.extensionName) == 0)
							{
								extensionFound = true;
								break;
							}
						}

						if (!extensionFound)
						{
							removedExtension = true;
							EXCIMER_LOG_WARN("[VULKAN] Extension not supported - {0}", extensionName);
						}

						return !extensionFound;
					}),
				extensions.end());

			return !removedExtension;
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

			//????VKInstance
			VkApplicationInfo appInfo = {};

			uint32_t sdkVersion = VK_HEADER_VERSION_COMPLETE;
			uint32_t driverVersion = 0;

			// if enumerateInstanceVersion  is missing, only vulkan 1.0 supported
			auto enumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

			if (enumerateInstanceVersion)
			{
				enumerateInstanceVersion(&driverVersion);
			}
			else
			{
				driverVersion = VK_API_VERSION_1_0;
			}

			// Choose supported version
			appInfo.apiVersion = Maths::Min(sdkVersion, driverVersion);

			// SDK not supported
			if (sdkVersion > driverVersion)
			{
				// Detect and log version
				std::string driverVersionStr = StringUtilities::ToString(VK_API_VERSION_MAJOR(driverVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_MINOR(driverVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_PATCH(driverVersion));
				std::string sdkVersionStr = StringUtilities::ToString(VK_API_VERSION_MAJOR(driverVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_MINOR(driverVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_PATCH(driverVersion));
				EXCIMER_LOG_WARN("Using Vulkan {0}. Please update your graphics drivers to support Vulkan {1}.", driverVersionStr, sdkVersionStr);
			}

			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Excimer";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Excimer";
			appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);

			VkInstanceCreateInfo createInfo = {};
			createInfo.pApplicationInfo = &appInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensionNames.size());
			createInfo.ppEnabledExtensionNames = m_InstanceExtensionNames.data();
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayerNames.size());
			createInfo.ppEnabledLayerNames = m_InstanceLayerNames.data();
			createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

			VkResult createResult = vkCreateInstance(&createInfo, nullptr, &s_VkInstance);
			if (createResult != VK_SUCCESS)
			{
				EXCIMER_LOG_CRITICAL("[VULKAN] Failed to create instance!");
			}

			volkLoadInstance(s_VkInstance);
		}

		VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
		{
			auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));

			if (func != nullptr)
			{
				return func(instance, pCreateInfo, pAllocator, pCallback);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		size_t VKContext::GetMinUniformBufferOffsetAlignment() const
		{
			return Graphics::VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.minUniformBufferOffsetAlignment;
		}


		void VKContext::SetupDebugCallback()
		{
			EXCIMER_PROFILE_FUNCTION();
			if (!EnableValidationLayers)
				return;

			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			createInfo.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(DebugCallback);

			VkResult result = CreateDebugReportCallbackEXT(s_VkInstance, &createInfo, nullptr, &m_DebugCallback);
			if (result != VK_SUCCESS)
			{
				EXCIMER_LOG_CRITICAL("[VULKAN] Failed to set up debug callback!");
			}
		}

		void VKContext::WaitIdle() const
		{
			vkDeviceWaitIdle(VKDevice::Get().GetDevice());
		}
	}
}