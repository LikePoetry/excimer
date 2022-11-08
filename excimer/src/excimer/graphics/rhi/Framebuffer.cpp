#include "hzpch.h"
#include "excimer/core/Reference.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "RenderPass.h"
#include "excimer/utilities/CombineHash.h"

#include "excimer/platform/vulkan/VK.h"

namespace Excimer
{
	namespace Graphics
	{
		Framebuffer* (*Framebuffer::CreateFunc)(const FramebufferDesc&) = nullptr;

		Framebuffer* Framebuffer::Create(const FramebufferDesc& framebufferDesc)
		{
			EXCIMER_ASSERT(CreateFunc, "No Framebuffer Create Function");

			return CreateFunc(framebufferDesc);
		}

		static std::unordered_map<std::size_t, SharedPtr<Framebuffer>> m_FramebufferCache;

		SharedPtr<Framebuffer> Framebuffer::Get(const FramebufferDesc& framebufferDesc)
		{
			size_t hash = 0;
			HashCombine(hash, framebufferDesc.attachmentCount, framebufferDesc.width, framebufferDesc.height, framebufferDesc.layer, framebufferDesc.renderPass, framebufferDesc.screenFBO);

			for (uint32_t i = 0; i < framebufferDesc.attachmentCount; i++)
			{
				HashCombine(hash, framebufferDesc.attachmentTypes[i]);

				if (framebufferDesc.attachments[i])
					HashCombine(hash, framebufferDesc.attachments[i]->GetImageHandle());
			}

			auto found = m_FramebufferCache.find(hash);
			if (found != m_FramebufferCache.end() && found->second)
			{
				return found->second;
			}

			auto framebuffer = SharedPtr<Framebuffer>(Create(framebufferDesc));
			m_FramebufferCache[hash] = framebuffer;
			return framebuffer;

		}

        void Framebuffer::ClearCache()
        {
            m_FramebufferCache.clear();
        }

        void Framebuffer::DeleteUnusedCache()
        {
            static const size_t keyDeleteSize = 256;
            static std::size_t keysToDelete[keyDeleteSize];
            std::size_t keysToDeleteCount = 0;

            for (auto&& [key, value] : m_FramebufferCache)
            {
                if (!value)
                {
                    keysToDelete[keysToDeleteCount] = key;
                    keysToDeleteCount++;
                }
                else if (value.GetCounter() && value.GetCounter()->GetReferenceCount() == 1)
                {
                    keysToDelete[keysToDeleteCount] = key;
                    keysToDeleteCount++;
                }

                if (keysToDeleteCount >= keyDeleteSize)
                    break;
            }

            for (std::size_t i = 0; i < keysToDeleteCount; i++)
            {
                m_FramebufferCache[keysToDelete[i]] = nullptr;
                m_FramebufferCache.erase(keysToDelete[i]);
            }
        }

        Framebuffer::~Framebuffer()
        {
        }
	}
}