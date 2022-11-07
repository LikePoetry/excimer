#pragma once
#include "Defintions.h"
#include "excimer/core/Asset.h"

namespace Excimer
{
	namespace Graphics
	{
		class EXCIMER_EXPORT Texture:public Asset
		{
		public:
			virtual ~Texture()
			{

			}

		protected:
			uint32_t m_Flags = 0;
			uint32_t m_BitsPerChannel = 8;
			uint32_t m_ChannelCount = 4;
		};

		class EXCIMER_EXPORT Texture2D : public Texture
		{
		protected:
			static Texture2D* (*CreateFunc)(TextureDesc parameters, uint32_t width, uint32_t height);
			static Texture2D* (*CreateFromSourceFunc)(uint32_t, uint32_t, void*, TextureDesc, TextureLoadOptions);
			static Texture2D* (*CreateFromFileFunc)(const std::string&, const std::string&, TextureDesc, TextureLoadOptions);
		};

		class EXCIMER_EXPORT TextureCube : public Texture
		{

		};

		class EXCIMER_EXPORT TextureDepth : public Texture
		{

		};

		class EXCIMER_EXPORT TextureDepthArray : public Texture
		{

		};
	}
}