#include "hzpch.h"
#include "VKFunctions.h"
#include "VKContext.h"
#include "VKRenderer.h"
#include "VKSwapChain.h"
#include "VKCommandBuffer.h"

/// <summary>
/// ��ʼ����ȾSDK���
/// </summary>
void Excimer::Graphics::Vulkan::MakeDefault()
{
	VKCommandBuffer::MakeDefault();
	VKContext::MakeDefault();
	VKRenderer::MakeDefault();
	VKSwapChain::MakeDefault();
}