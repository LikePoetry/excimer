#include "hzpch.h"
#include "VKFunctions.h"
#include "VKContext.h"
#include "VKRenderer.h"
#include "VKSwapChain.h"

/// <summary>
/// ��ʼ����ȾSDK���
/// </summary>
void Excimer::Graphics::Vulkan::MakeDefault()
{
	VKContext::MakeDefault();
	VKRenderer::MakeDefault();
	VKSwapChain::MakeDefault();
}