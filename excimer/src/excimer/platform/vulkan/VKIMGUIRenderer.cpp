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

static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	printf("VkResult %d\n", err);
	if (err < 0)
		abort();
}

namespace Excimer
{
	namespace Graphics
	{
		VKIMGUIRenderer::VKIMGUIRenderer(uint32_t width, uint32_t height, bool clearScreen)
			: m_Framebuffers{}
			, m_Renderpass(nullptr)
			, m_FontTexture(nullptr)
		{
			EXCIMER_PROFILE_FUNCTION();

			m_WindowHandle = nullptr;
			m_Width = width;
			m_Height = height;
			m_ClearScreen = clearScreen;
		}

		VKIMGUIRenderer::~VKIMGUIRenderer()
		{
			EXCIMER_PROFILE_FUNCTION();

			for (int i = 0; i < Renderer::GetMainSwapChain()->GetSwapChainBufferCount(); i++)
			{
				delete m_Framebuffers[i];
			}

			delete m_Renderpass;
			delete m_FontTexture;

			VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

			for (int i = 0; i < Renderer::GetMainSwapChain()->GetSwapChainBufferCount(); i++)
			{
				ImGui_ImplVulkanH_Frame* fd = &g_WindowData.Frames[i];
				auto fence = fd->Fence;
				auto alloc = g_Allocator;
				auto commandPool = fd->CommandPool;

				deletionQueue.PushFunction([fence, commandPool, alloc] {
					vkDestroyFence(VKDevice::Get().GetDevice(), fence, alloc);
					vkDestroyCommandPool(VKDevice::Get().GetDevice(), commandPool, alloc); });
			}
			auto descriptorPool = g_DescriptorPool;

			deletionQueue.PushFunction([descriptorPool] {
				vkDestroyDescriptorPool(VKDevice::Get().GetDevice(), descriptorPool, nullptr);
				ImGui_ImplVulkan_Shutdown(); });
		}

		void VKIMGUIRenderer::SetupVulkanWindowData(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
		{
			EXCIMER_PROFILE_FUNCTION();

			// Create Descriptor Pool
			{
				VkDescriptorPoolSize pool_sizes[] = {
					{VK_DESCRIPTOR_TYPE_SAMPLER, 100000},
					{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100000},
					{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100000},
					{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100000},
					{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100000},
					{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100000},
					{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100000},
					{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100000},
					{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100000},
					{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100000},
					{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100000} };
				VkDescriptorPoolCreateInfo pool_info = {};
				pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
				pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
				pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
				pool_info.pPoolSizes = pool_sizes;
				VkResult err = vkCreateDescriptorPool(VKDevice::Get().GetDevice(), &pool_info, g_Allocator, &g_DescriptorPool);
				check_vk_result(err);
			}

			wd->Surface = surface;
			wd->ClearEnable = m_ClearScreen;

			auto swapChain = static_cast<VKSwapChain*>(VKRenderer::GetMainSwapChain());
			wd->Swapchain = swapChain->GetSwapChain();
			wd->Width = width;
			wd->Height = height;

			wd->ImageCount = static_cast<uint32_t>(swapChain->GetSwapChainBufferCount());

			TextureType textureTypes[1] = { TextureType::COLOUR };

			Texture* textures[1] = { swapChain->GetImage(0) };

			Graphics::RenderPassDesc renderPassDesc;
			renderPassDesc.attachmentCount = 1;
			renderPassDesc.attachmentTypes = textureTypes;
			renderPassDesc.clear = m_ClearScreen;
			renderPassDesc.attachments = textures;

			m_Renderpass = new VKRenderPass(renderPassDesc);
			wd->RenderPass = m_Renderpass->GetHandle();

			wd->Frames = (ImGui_ImplVulkanH_Frame*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_Frame) * wd->ImageCount);
			// wd->FrameSemaphores = (ImGui_ImplVulkanH_FrameSemaphores*)IM_ALLOC(sizeof(ImGui_ImplVulkanH_FrameSemaphores) * wd->ImageCount);
			memset(wd->Frames, 0, sizeof(wd->Frames[0]) * wd->ImageCount);
			// memset(wd->FrameSemaphores, 0, sizeof(wd->FrameSemaphores[0]) * wd->ImageCount);

			// Create The Image Views
			{
				for (uint32_t i = 0; i < wd->ImageCount; i++)
				{
					auto scBuffer = (VKTexture2D*)swapChain->GetImage(i);
					wd->Frames[i].Backbuffer = scBuffer->GetImage();
					wd->Frames[i].BackbufferView = scBuffer->GetImageView();
				}
			}

			TextureType attachmentTypes[1];
			attachmentTypes[0] = TextureType::COLOUR;

			Texture* attachments[1];
			FramebufferDesc frameBufferDesc{};
			frameBufferDesc.width = wd->Width;
			frameBufferDesc.height = wd->Height;
			frameBufferDesc.attachmentCount = 1;
			frameBufferDesc.renderPass = m_Renderpass;
			frameBufferDesc.attachmentTypes = attachmentTypes;
			frameBufferDesc.screenFBO = true;

			for (uint32_t i = 0; i < Renderer::GetMainSwapChain()->GetSwapChainBufferCount(); i++)
			{
				attachments[0] = Renderer::GetMainSwapChain()->GetImage(i);
				frameBufferDesc.attachments = attachments;

				m_Framebuffers[i] = new VKFramebuffer(frameBufferDesc);
				wd->Frames[i].Framebuffer = m_Framebuffers[i]->GetFramebuffer();
			}
		}

		void VKIMGUIRenderer::Init()
		{
			EXCIMER_PROFILE_FUNCTION();
			int w, h;
			w = (int)m_Width;
			h = (int)m_Height;
			ImGui_ImplVulkanH_Window* wd = &g_WindowData;
			VkSurfaceKHR surface = VKRenderer::GetMainSwapChain()->GetSurface();
			SetupVulkanWindowData(wd, surface, w, h);

			// Setup Vulkan binding
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = VKContext::GetVKInstance();
			init_info.PhysicalDevice = VKDevice::Get().GetGPU();
			init_info.Device = VKDevice::Get().GetDevice();
			init_info.QueueFamily = VKDevice::Get().GetPhysicalDevice()->GetGraphicsQueueFamilyIndex();
			init_info.Queue = VKDevice::Get().GetGraphicsQueue();
			init_info.PipelineCache = VKDevice::Get().GetPipelineCache();
			init_info.DescriptorPool = g_DescriptorPool;
			init_info.Allocator = g_Allocator;
			init_info.CheckVkResultFn = NULL;
			init_info.MinImageCount = 2;
			init_info.ImageCount = (uint32_t)Renderer::GetMainSwapChain()->GetSwapChainBufferCount();
			ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);
			// Upload Fonts
			{
				ImGuiIO& io = ImGui::GetIO();

				unsigned char* pixels;
				int width, height;
				io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

				m_FontTexture = new VKTexture2D(width, height, pixels, TextureDesc(TextureFilter::NEAREST, TextureFilter::NEAREST));
				io.Fonts->TexID = (ImTextureID)m_FontTexture->GetHandle();
			}
		}

		void VKIMGUIRenderer::NewFrame()
		{
		}

		void VKIMGUIRenderer::FrameRender(ImGui_ImplVulkanH_Window* wd)
		{
			EXCIMER_PROFILE_FUNCTION();

			EXCIMER_PROFILE_GPU("ImGui Pass");

			wd->FrameIndex = VKRenderer::GetMainSwapChain()->GetCurrentImageIndex();
			auto& descriptorImageMap = ImGui_ImplVulkan_GetDescriptorImageMap();

			{
				auto draw_data = ImGui::GetDrawData();
				for (int n = 0; n < draw_data->CmdListsCount; n++)
				{
					const ImDrawList* cmd_list = draw_data->CmdLists[n];
					for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
					{
						const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

						if ((Texture*)pcmd->TextureId)
						{
							if (((Texture*)pcmd->TextureId)->GetType() == TextureType::COLOUR)
							{
								auto texture = (VKTexture2D*)pcmd->TextureId;
								texture->TransitionImage(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, (VKCommandBuffer*)Renderer::GetMainSwapChain()->GetCurrentCommandBuffer());
								descriptorImageMap[pcmd->TextureId] = texture->GetDescriptor();
							}
							else if (((Texture*)pcmd->TextureId)->GetType() == TextureType::DEPTH)
							{
								auto texture = (VKTextureDepth*)pcmd->TextureId;
								texture->TransitionImage(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, (VKCommandBuffer*)Renderer::GetMainSwapChain()->GetCurrentCommandBuffer());
								descriptorImageMap[pcmd->TextureId] = texture->GetDescriptor();
							}
							else if (((Texture*)pcmd->TextureId)->GetType() == TextureType::DEPTHARRAY)
							{
								auto texture = (VKTextureDepthArray*)pcmd->TextureId;
								texture->TransitionImage(VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, (VKCommandBuffer*)Renderer::GetMainSwapChain()->GetCurrentCommandBuffer());
								descriptorImageMap[pcmd->TextureId] = texture->GetDescriptor();
							}
						}
					}
				}
			}

			ImGui_ImplVulkan_CreateDescriptorSets(ImGui::GetDrawData(), VKRenderer::GetMainSwapChain()->GetCurrentBufferIndex());

			float clearColour[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
			m_Renderpass->BeginRenderpass(Renderer::GetMainSwapChain()->GetCurrentCommandBuffer(), clearColour, m_Framebuffers[wd->FrameIndex], Graphics::SubPassContents::INLINE, wd->Width, wd->Height);

			{
				EXCIMER_PROFILE_SCOPE("ImGui Vulkan RenderDrawData");
				// Record Imgui Draw Data and draw funcs into command buffer
				ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), ((VKCommandBuffer*)Renderer::GetMainSwapChain()->GetCurrentCommandBuffer())->GetHandle(), VK_NULL_HANDLE, VKRenderer::GetMainSwapChain()->GetCurrentBufferIndex());
			}
			m_Renderpass->EndRenderpass(Renderer::GetMainSwapChain()->GetCurrentCommandBuffer());
		}

		void VKIMGUIRenderer::Render(Excimer::Graphics::CommandBuffer* commandBuffer)
		{
			EXCIMER_PROFILE_FUNCTION();

			ImGui::Render();
			FrameRender(&g_WindowData);
		}

		void VKIMGUIRenderer::OnResize(uint32_t width, uint32_t height)
		{
			EXCIMER_PROFILE_FUNCTION();

			auto* wd = &g_WindowData;
			auto swapChain = static_cast<VKSwapChain*>(VKRenderer::GetMainSwapChain());
			wd->Swapchain = swapChain->GetSwapChain();
			for (uint32_t i = 0; i < wd->ImageCount; i++)
			{
				auto scBuffer = (VKTexture2D*)swapChain->GetImage(i);
				wd->Frames[i].Backbuffer = scBuffer->GetImage();
				wd->Frames[i].BackbufferView = scBuffer->GetImageView();
			}

			wd->Width = width;
			wd->Height = height;

			for (uint32_t i = 0; i < wd->ImageCount; i++)
			{
				delete m_Framebuffers[i];
			}
			// Create Framebuffer
			TextureType attachmentTypes[1];
			attachmentTypes[0] = TextureType::COLOUR;

			Texture* attachments[1];
			FramebufferDesc frameBufferDesc{};
			frameBufferDesc.width = wd->Width;
			frameBufferDesc.height = wd->Height;
			frameBufferDesc.attachmentCount = 1;
			frameBufferDesc.renderPass = m_Renderpass;
			frameBufferDesc.attachmentTypes = attachmentTypes;
			frameBufferDesc.screenFBO = true;

			for (uint32_t i = 0; i < Renderer::GetMainSwapChain()->GetSwapChainBufferCount(); i++)
			{
				attachments[0] = Renderer::GetMainSwapChain()->GetImage(i);
				frameBufferDesc.attachments = attachments;

				m_Framebuffers[i] = new VKFramebuffer(frameBufferDesc);
				wd->Frames[i].Framebuffer = m_Framebuffers[i]->GetFramebuffer();
			}
		}

		void VKIMGUIRenderer::Clear()
		{
			// ImGui_ImplVulkan_ClearDescriptors();
		}

		void VKIMGUIRenderer::MakeDefault()
		{
			CreateFunc = CreateFuncVulkan;
		}

		IMGUIRenderer* VKIMGUIRenderer::CreateFuncVulkan(uint32_t width, uint32_t height, bool clearScreen)
		{
			return new VKIMGUIRenderer(width, height, clearScreen);
		}

		void VKIMGUIRenderer::RebuildFontTexture()
		{
			EXCIMER_PROFILE_FUNCTION();

			// Upload Fonts
			{
				ImGuiIO& io = ImGui::GetIO();

				unsigned char* pixels;
				int width, height;
				io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

				m_FontTexture = new VKTexture2D(width, height, pixels, TextureDesc(TextureFilter::NEAREST, TextureFilter::NEAREST, TextureWrap::REPEAT));
				io.Fonts->TexID = (ImTextureID)m_FontTexture->GetHandle();
			}
		}
	}
}

