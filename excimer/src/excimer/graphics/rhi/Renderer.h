#pragma once
#include "excimer/core/Application.h"
#include "excimer/core/os/Window.h"

namespace Excimer
{
	namespace Graphics
	{
		struct RenderAPICapabilities
		{
			std::string Vendor;
			std::string Renderer;
			std::string Version;

			int MaxSamples = 0;
			float MaxAnisotropy = 0.0f;
			int MaxTextureUnits = 0;
			int UniformBufferOffsetAlignment = 0;
			bool WideLines = false;
			bool SupportCompute = false;
		};

		class EXCIMER_EXPORT Renderer
		{
		public:
			Renderer() = default;
			~Renderer() = default;

			static RenderAPICapabilities& GetCapabilities()
			{
				static RenderAPICapabilities capabilities;
				return capabilities;
			}

		protected:
			static Renderer* (*CreateFunc)();

			static Renderer* s_Instance;
		};
	}
}