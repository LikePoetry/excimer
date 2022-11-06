#include "hzpch.h"
#include "VKFunctions.h"
#include "VKContext.h"
#include "VKRenderer.h"
#include "VKSwapChain.h"

/// <summary>
/// 初始化渲染SDK组件
/// </summary>
void Excimer::Graphics::Vulkan::MakeDefault()
{
	VKContext::MakeDefault();
	VKRenderer::MakeDefault();
	VKSwapChain::MakeDefault();
}