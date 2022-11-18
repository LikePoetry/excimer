#include "hzpch.h"
#include "TextureMatrixComponent.h"

namespace Excimer
{
    TextureMatrixComponent::TextureMatrixComponent(const glm::mat4& matrix)
        : m_TextureMatrix(matrix)
    {
    }

    void TextureMatrixComponent::OnImGui()
    {
    }
}
