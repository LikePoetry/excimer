#include "hzpch.h"
#include "VKFunctions.h"
#include "VKContext.h"
#include "VKRenderer.h"
#include "VKSwapChain.h"
#include "VKCommandBuffer.h"
#include "VKDescriptorSet.h"
#include "VKFramebuffer.h"
#include "VKIMGUIRenderer.h"
#include "VKShader.h"
#include "VKRenderDevice.h"
#include "VKIndexBuffer.h"
#include "VKVertexBuffer.h"


/// <summary>
/// 初始化渲染SDK组件
/// </summary>
void Excimer::Graphics::Vulkan::MakeDefault()
{
	VKCommandBuffer::MakeDefault();
	VKContext::MakeDefault();
	VKDescriptorSet::MakeDefault();
	VKFramebuffer::MakeDefault();
	VKIMGUIRenderer::MakeDefault();
	VKIndexBuffer::MakeDefault();

	VKPipeline::MakeDefault();
	VKRenderDevice::MakeDefault();
	VKRenderer::MakeDefault();
	VKRenderPass::MakeDefault();
	VKShader::MakeDefault();
	VKSwapChain::MakeDefault();
	VKTexture2D::MakeDefault();
	VKTextureCube::MakeDefault();
	VKTextureDepth::MakeDefault();
	VKTextureDepthArray::MakeDefault();
	VKUniformBuffer::MakeDefault();
	VKVertexBuffer::MakeDefault();
}