#pragma once
#include "excimer/maths/Maths.h"
#include "Renderable2D.h"
#include "excimer/core/VFS.h"

#include <cereal/cereal.hpp>

namespace Excimer
{
	namespace Graphics
	{
		class Texture2D;

		class EXCIMER_EXPORT Sprite:public Renderable2D
		{
        public:
            Sprite(const glm::vec2& position = glm::vec2(0.0f, 0.0f), const glm::vec2& scale = glm::vec2(1.0f, 1.0f), const glm::vec4& colour = glm::vec4(1.0f));
            Sprite(const SharedPtr<Texture2D>& texture, const glm::vec2& position, const glm::vec2& scale, const glm::vec4& colour);
            virtual ~Sprite();
            void SetPosition(const glm::vec2& vector2) { m_Position = vector2; };
            void SetColour(const glm::vec4& colour) { m_Colour = colour; }
            void SetScale(const glm::vec2& scale) { m_Scale = scale; }

            void SetSpriteSheet(const SharedPtr<Texture2D>& texture, const glm::vec2& index, const glm::vec2& cellSize, const glm::vec2& spriteSize);
            void SetTexture(const SharedPtr<Texture2D>& texture) { m_Texture = texture; }

            void SetTextureFromFile(const std::string& filePath);

            template <typename Archive>
            void save(Archive& archive) const
            {
                std::string newPath = "";
                if (m_Texture)
                {
                    VFS::Get().AbsoulePathToVFS(m_Texture->GetFilepath(), newPath);
                }

                archive(cereal::make_nvp("TexturePath", newPath),
                    cereal::make_nvp("Position", m_Position),
                    cereal::make_nvp("Scale", m_Scale),
                    cereal::make_nvp("Colour", m_Colour));
            }

            template <typename Archive>
            void load(Archive& archive)
            {
                std::string textureFilePath;
                archive(cereal::make_nvp("TexturePath", textureFilePath),
                    cereal::make_nvp("Position", m_Position),
                    cereal::make_nvp("Scale", m_Scale),
                    cereal::make_nvp("Colour", m_Colour));

                if (!textureFilePath.empty())
                    m_Texture = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::CreateFromFile("sprite", textureFilePath));
            }
		};
	}
}