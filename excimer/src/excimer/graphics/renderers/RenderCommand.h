#pragma once
#include "excimer/graphics/Mesh.h"
#include "excimer/graphics/RHI/Shader.h"

namespace Excimer
{
    namespace Graphics
    {
        class Material;
        class Pipeline;

        struct EXCIMER_EXPORT RenderCommand
        {
            Mesh* mesh = nullptr;
            Material* material = nullptr;
            Pipeline* pipeline = nullptr;
            glm::mat4 transform;
            glm::mat4 textureMatrix;
            bool animated = false;
        };
    }
}