#pragma once
#include "Defintions.h"
#include <glm/vec4.hpp>

#include "excimer/core/Core.h"
#include "excimer/core/Reference.h"

namespace Excimer
{
	namespace Graphics
	{
		class EXCIMER_EXPORT RenderPass
		{
		public:
			virtual ~RenderPass();
			static RenderPass* Create(const RenderPassDesc& renderPassDesc);
			static SharedPtr<RenderPass> Get(const RenderPassDesc& renderPassDesc);
			static void ClearCache();
			static void DeleteUnusedCache();
			virtual void BeginRenderpass(CommandBuffer* commandBuffer, float* clearColour, Framebuffer* frame, SubPassContents contents, uint32_t width, uint32_t height) const = 0;
			virtual void EndRenderpass(CommandBuffer* commandBuffer) = 0;
			virtual int GetAttachmentCount() const = 0;

		protected:
			static RenderPass* (*CreateFunc)(const RenderPassDesc&);
		};
	}
}