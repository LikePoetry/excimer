#pragma once
#include "excimer/graphics/rhi/GraphicsContext.h"

#include "excimer/platform/vulkan/VKDevice.h"
#include "excimer/platform/vulkan/VKCommandBuffer.h"

//#define EXCIMER_PROFILE_GPU(name) TracyVkZone(Slight::Graphics::VKDevice::Get().GetTracyContext(), static_cast<Slight::Graphics::VKCommandBuffer*>(Renderer::GetMainSwapChain()->GetCurrentCommandBuffer())->GetHandle(), name)

#define EXCIMER_PROFILE_GPU(name)