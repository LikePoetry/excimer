#include "hzpch.h"
#include "VKIMGUIRenderer.h"
#include <imgui/imgui.h>

#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#include <imgui/backends/imgui_impl_vulkan.h>

#include "VKRenderer.h"
#include "VKRenderPass.h"
#include "VKTexture.h"
#include "excimer/graphics/rhi/GPUProfile.h"

static ImGui_ImplVulkanH_Window g_WindowData;
static VkAllocationCallbacks* g_Allocator = nullptr;
static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;

