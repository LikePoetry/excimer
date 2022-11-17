#pragma once
#include "excimer/graphics/rhi/GraphicsContext.h"
#include "VK.h"
#include "excimer/core/Reference.h"
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"
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
			void Present() override;
			void Unload();

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags,
				VkDebugReportObjectTypeEXT objType,
				uint64_t sourceObj,
				size_t location,
				int32_t msgCode,
				const char* pLayerPrefix,
				const char* pMsg,
				void* userData);

			static VkInstance GetVKInstance() { return s_VkInstance; }

			size_t GetMinUniformBufferOffsetAlignment() const override;

			bool FlipImGUITexture() const override { return true; }
			void WaitIdle() const override;
			void OnImGui() override {};


			float GetGPUMemoryUsed() override { return 0.0f; };
			float GetTotalGPUMemory() override { return 0.0f; };

			const std::vector<const char*>& GetLayerNames() const { return m_InstanceLayerNames; }
			const std::vector<const char*>& GetExtensionNames() const { return m_InstanceExtensionNames; }

			static void MakeDefault();

			struct DeletionQueue
			{
				std::deque<std::function<void()>> m_Deletors;

				template <typename F>
				void PushFunction(F&& function)
				{
					EXCIMER_ASSERT(sizeof(F) < 200, "Lambda too large");
					m_Deletors.push_back(function);
				}

				void Flush()
				{
					for (auto it = m_Deletors.rbegin(); it != m_Deletors.rend(); it++)
					{
						(*it)();
					}

					m_Deletors.clear();
				}
			};

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