#pragma once
#include "Renderer.h"
#include "Defintions.h"
#include "Shader.h"

namespace Excimer
{
	namespace Graphics
	{
		struct PipelineDesc
		{
			SharedPtr<Shader> shader;

			CullMode cullMode = CullMode::BACK;
			PolygonMode polygonMode = PolygonMode::FILL;
			DrawType drawType = DrawType::TRIANGLE;
			BlendMode blendMode = BlendMode::None;

			bool transparencyEnabled = false;
			bool depthBiasEnabled = false;
			bool swapchainTarget = false;
			bool clearTargets = false;

			std::array<Texture*, MAX_RENDER_TARGETS> colourTargets = {};

			Texture* cubeMapTarget = nullptr;
			Texture* depthTarget = nullptr;
			Texture* depthArrayTarget = nullptr;
			float clearColour[4] = { 0.2f,0.2f,0.2f,1.0f };
			float lineWidth = 1.0f;
			float depthBiasConstantFactor = 0.0f;
			float depthBiasSlopeFactor = 0.0f;
			int cubeMapIndex = 0;
			int mipIndex = -1;
		};

		class EXCIMER_EXPORT Pipeline
		{
		public:
			static Pipeline* Create(const PipelineDesc& pipelineDesc);
			static SharedPtr<Pipeline> Get(const PipelineDesc& pipelineDesc);
			static void ClearCache();
			static void DeleteUnusedCache();

			virtual ~Pipeline() = default;

			virtual void Bind(CommandBuffer* commandBuffer, uint32_t layer = 0) = 0;
			virtual void End(CommandBuffer* commandBuffer){}
			virtual void ClearRenderTargets(CommandBuffer* commandBuffer){}
			virtual Shader* GetShader() const = 0;

			uint32_t GetWidth();
			uint32_t GetHeight();

		protected:
			static Pipeline* (*CreateFunc)(const PipelineDesc&);
			PipelineDesc m_Description;
		};
	}
}