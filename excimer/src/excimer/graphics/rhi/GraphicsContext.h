#pragma once
#include "excimer/core/Core.h"

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
	}
}