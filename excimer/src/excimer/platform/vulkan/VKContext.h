#pragma once
#include "excimer/graphics/rhi/GraphicsContext.h"
#include "VK.h"
#include "excimer/core/Reference.h"

#include <deque>

#include <vulkan/vk_mem_alloc.h>

const bool EnableValidationLayers = true;

namespace Excimer
{
	namespace Graphics
	{
		class VKContext :public GraphicsContext
		{
		public:
			VKContext();
			~VKContext();

			void Init() override;

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
				VkDebugReportObjectTypeEXT objType,
				uint64_t sourceObj,
				size_t location,
				int32_t msgCode,
				const char* pLayerPrefix,
				const char* pMsg,
				void* userData);

			static VkInstance GetVKInstance() { return s_VkInstance; }
			static void MakeDefault();

		protected:
			static GraphicsContext* CreateFuncVulkan();

			void CreateInstance();
			void SetupDebugCallback();
			bool CheckValidationLayerSupport(std::vector<const char*>& validationLayers);
			bool CheckExtensionSupport(std::vector<const char*>& extensions);

			static const std::vector<const char*> GetRequiredExtensions();
			const std::vector<const char*> GetRequiredLayers() const;

		private:
			static VkInstance s_VkInstance;
			VkDebugReportCallbackEXT m_DebugCallback = VK_NULL_HANDLE;

			std::vector<VkLayerProperties> m_InstanceLayers;
			std::vector<VkExtensionProperties> m_InstanceExtensions;

			std::vector<const char*> m_InstanceLayerNames;
			std::vector<const char*> m_InstanceExtensionNames;

			bool m_StandardValidationLayer = false;
			bool m_AssistanceLayer = false;

		};
	}
}