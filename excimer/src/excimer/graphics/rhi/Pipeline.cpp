#include "hzpch.h"
#include "Pipeline.h"

#include "excimer/core/Engine.h"

#include "SwapChain.h"
#include "Texture.h"
#include "Renderer.h"

#include "excimer/utilities/CombineHash.h"
#include "excimer/graphics/rhi/GraphicsContext.h"

#include "excimer/platform/vulkan/VK.h"

namespace Excimer
{
	namespace Graphics
	{
		struct PipelineAsset
		{
			SharedPtr<Pipeline> pipeline;
			float timeSinceLastAccessed;
		};
		static std::unordered_map<std::size_t, PipelineAsset> m_PipelineCache;
		static const float m_CacheLifeTime = 0.1f;

		Pipeline* (*Pipeline::CreateFunc)(const PipelineDesc&) = nullptr;

		Pipeline* Pipeline::Create(const PipelineDesc& pipelineDesc)
		{
			EXCIMER_PROFILE_FUNCTION();
			EXCIMER_ASSERT(CreateFunc, "No Pipeline Create Function");
			return CreateFunc(pipelineDesc);
		}

		SharedPtr<Pipeline> Pipeline::Get(const PipelineDesc& pipelineDesc)
		{
			EXCIMER_PROFILE_FUNCTION();
			size_t hash = 0;
			HashCombine(hash, pipelineDesc.shader.get(), pipelineDesc.cullMode, pipelineDesc.depthBiasEnabled, pipelineDesc.drawType, pipelineDesc.polygonMode, pipelineDesc.transparencyEnabled);

			for (auto texture : pipelineDesc.colourTargets)
			{
				if (texture)
				{
					HashCombine(hash, texture, texture->GetWidth(), texture->GetHeight(), texture->GetImageHandle());
				}
			}

			if (pipelineDesc.depthTarget)
			{
				HashCombine(hash, pipelineDesc.depthTarget, pipelineDesc.depthTarget->GetImageHandle());
			}

			if (pipelineDesc.depthArrayTarget)
			{
				HashCombine(hash, pipelineDesc.depthArrayTarget, pipelineDesc.depthArrayTarget->GetImageHandle());
			}

			HashCombine(hash, pipelineDesc.clearTargets);
			HashCombine(hash, pipelineDesc.swapchainTarget);
			HashCombine(hash, pipelineDesc.lineWidth);
			HashCombine(hash, pipelineDesc.depthBiasConstantFactor);
			HashCombine(hash, pipelineDesc.depthBiasSlopeFactor);
			HashCombine(hash, pipelineDesc.cubeMapIndex);
			HashCombine(hash, pipelineDesc.cubeMapTarget);
			HashCombine(hash, pipelineDesc.mipIndex);

			if (pipelineDesc.swapchainTarget)
			{
				// Add one swapchain image to hash
				auto texture = Renderer::GetMainSwapChain()->GetCurrentImage();
				if (texture)
				{
					HashCombine(hash, texture->GetImageHandle());
				}
			}

			auto found = m_PipelineCache.find(hash);
			if (found != m_PipelineCache.end() && found->second.pipeline)
			{
				found->second.timeSinceLastAccessed = (float)Engine::GetTimeStep().GetElapsedSeconds();
				return found->second.pipeline;
			}

			SharedPtr<Pipeline> pipeline = SharedPtr<Pipeline>(Create(pipelineDesc));
			m_PipelineCache[hash] = { pipeline, (float)Engine::GetTimeStep().GetElapsedSeconds() };
			return pipeline;
		}

		void Pipeline::ClearCache()
		{
			m_PipelineCache.clear();
		}

		void Pipeline::DeleteUnusedCache()
		{
			EXCIMER_PROFILE_FUNCTION();

			static std::size_t keysToDelete[256];
			std::size_t keysToDeleteCount = 0;

			for (auto&& [key, value] : m_PipelineCache)
			{
				if (value.pipeline && value.pipeline.GetCounter()->GetReferenceCount() == 1 && (Engine::GetTimeStep().GetElapsedSeconds() - value.timeSinceLastAccessed) > m_CacheLifeTime)
				{
					keysToDelete[keysToDeleteCount] = key;
					keysToDeleteCount++;
				}

				if (keysToDeleteCount >= 256)
					break;
			}

			for (std::size_t i = 0; i < keysToDeleteCount; i++)
			{
				m_PipelineCache[keysToDelete[i]].pipeline = nullptr;
				m_PipelineCache.erase(keysToDelete[i]);
			}
		}

		uint32_t Pipeline::GetWidth()
		{
			if (m_Description.swapchainTarget)
			{
				return Renderer::GetMainSwapChain()->GetCurrentImage()->GetWidth();
			}

			if (m_Description.colourTargets[0])
			{
				return m_Description.mipIndex > 0 ? m_Description.colourTargets[0]->GetWidth(m_Description.mipIndex) : m_Description.colourTargets[0]->GetWidth();
			}

			if (m_Description.depthTarget)
				return m_Description.depthTarget->GetWidth();

			if (m_Description.depthArrayTarget)
				return m_Description.depthArrayTarget->GetWidth();

			if (m_Description.cubeMapTarget)
				return m_Description.cubeMapTarget->GetWidth();

			EXCIMER_LOG_WARN("Invalid pipeline width");

			return 0;
			}

		uint32_t Pipeline::GetHeight()
		{
			if (m_Description.swapchainTarget)
			{
				return Renderer::GetMainSwapChain()->GetCurrentImage()->GetHeight();
			}

			if (m_Description.colourTargets[0])
			{
				// TODO
				return m_Description.mipIndex > 0 ? m_Description.colourTargets[0]->GetHeight(m_Description.mipIndex) : m_Description.colourTargets[0]->GetHeight();
		}

			if (m_Description.depthTarget)
				return m_Description.depthTarget->GetHeight();

			if (m_Description.depthArrayTarget)
				return m_Description.depthArrayTarget->GetHeight();

			if (m_Description.cubeMapTarget)
				return m_Description.cubeMapTarget->GetHeight();

			EXCIMER_LOG_WARN("Invalid pipeline height");

			return 0;
		}
	}
}