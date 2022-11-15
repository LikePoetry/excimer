#pragma once
#include "excimer/core/Core.h"
#include "excimer/core/Asset.h"
#include "excimer/core/Reference.h"
#include <string>

namespace Excimer
{
	struct MSDFData;

	namespace Graphics
	{
		class Texture2D;

		class Font:public Asset
		{
			class FontHolder;

		public:
			Font(const std::string& filepath);
			Font(uint8_t* data, uint32_t dataSize, const std::string& name);

			virtual ~Font();

			SharedPtr<Graphics::Texture2D> GetFontAtlas() const { return m_TextureAtlas; }
			const MSDFData* GetMSDFData() const { return m_MSDFData; }
			const std::string& GetFilePath() const { return m_FilePath; }

			void Init();

			static void InitDefaultFont();
			static void ShutdownDefaultFont();
			static SharedPtr<Font> GetDefaultFont();

			SET_ASSET_TYPE(AssetType::Font);

		private:
			std::string m_FilePath;
			SharedPtr<Graphics::Texture2D> m_TextureAtlas;
			MSDFData* m_MSDFData = nullptr;
			uint8_t* m_FontData;
			uint32_t m_FontDataSize;

		private:
			static SharedPtr<Font> s_DefaultFont;
		};
	}
}