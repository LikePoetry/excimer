#include "hzpch.h"
#include "ModelComponent.h"
#include "excimer/core/Application.h"

namespace Excimer::Graphics
{
    ModelComponent::ModelComponent(const std::string& path)
    {
        LoadFromLibrary(path);
    }

    void ModelComponent::LoadFromLibrary(const std::string& path)
    {
        ModelRef = Application::Get().GetModelLibrary()->GetResource(path);
    }

}
