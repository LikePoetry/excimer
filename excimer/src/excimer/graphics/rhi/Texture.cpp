#include "hzpch.h"
#include "Texture.h"

namespace Excimer
{
	namespace Graphics 
	{
		Texture2D* (*Texture2D::CreateFunc)(TextureDesc, uint32_t, uint32_t) = nullptr;
		Texture2D* (*Texture2D::CreateFromSourceFunc)(uint32_t, uint32_t, void*, TextureDesc, TextureLoadOptions) = nullptr;
		Texture2D* (*Texture2D::CreateFromFileFunc)(const std::string&, const std::string&, TextureDesc, TextureLoadOptions) = nullptr;
	}
}