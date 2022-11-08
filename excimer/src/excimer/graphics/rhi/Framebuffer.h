#pragma once
#include <glm/vec4.hpp>
#include "Defintions.h"

namespace Excimer
{
	namespace Graphics
	{
		enum class CubeFace
		{
			PositiveX,
			NegativeX,
			PositiveY,
			NegativeY,
			PositiveZ,
			NegativeZ
		};

		struct FramebufferDesc
		{
			uint32_t width;
			uint32_t height;
			uint32_t layer = 0;
			uint32_t attachmentCount;
			uint32_t msaaLevel;
			int mipIndex = -1;
			bool screenFBO = false;
			Texture** attachments;
			TextureType* attachmentTypes;
			Graphics::RenderPass* renderPass;
		};

		class EXCIMER_EXPORT Framebuffer
		{

		};
	}
}
