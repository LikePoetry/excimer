#pragma once
#include <string>

namespace Excimer
{
    void EmbedTexture(const std::string& texFilePath, const std::string& outPath, const std::string& arrayName);
    void EmbedShader(const std::string& shaderPath);

}