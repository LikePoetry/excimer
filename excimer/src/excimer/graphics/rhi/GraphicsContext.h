#pragma once
#include "excimer/core/Core.h"
#include "excimer/core/ExLog.h"

namespace Excimer
{
	namespace Graphics
	{
		enum class RenderAPI :uint32_t
		{
			OPENGL=0,
			VULKAN,
			DIRECT3D,	// Unsupported
			METAL,		// Unsupported
			NONE,		// Unsupported
		};

		class EXCIMER_EXPORT GraphicsContext
		{
		public:
			virtual ~GraphicsContext();

			static RenderAPI GetRenderAPI() { return s_RenderAPI; }
			static void SetRenderAPI(RenderAPI api);

			virtual void Init() = 0;
			virtual void WaitIdle() const = 0;

			static GraphicsContext* Create();
		protected:
			static GraphicsContext* (*CreateFunc)();

			static RenderAPI s_RenderAPI;
		};
	}
}