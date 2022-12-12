#pragma once
#include "IRenderer.h"

namespace Excimer
{
	namespace Graphics
	{
		class Shader;
		class EXCIMER_EXPORT GridRenderer:public IRenderer
		{
		public:
			GridRenderer(uint32_t width, uint32_t height);
			~GridRenderer();

			void Init() override;
			void BeginScene(Scene* scene, Camera* overrideCamera, Maths::Transform* overrideCameraTransform) override;
			void OnResize(uint32_t width, uint32_t height) override;
			void CreateGraphicsPipeline();
			void UpdateUniformBuffer();

			void Begin() override;
		};
	}
}