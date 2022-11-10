#include "hzpch.h"
#include "Renderer.h"

namespace Excimer
{
	namespace Graphics
	{
		Renderer* (*Renderer::CreateFunc)() = nullptr;

		Renderer* Renderer::s_Instance = nullptr;

        void Renderer::Init(bool loadEmbeddedShaders)
        {
            EXCIMER_ASSERT(CreateFunc, "No Renderer Create Function");
            EXCIMER_PROFILE_FUNCTION();
            s_Instance = CreateFunc();
            s_Instance->InitInternal();
            s_Instance->LoadEngineShaders(loadEmbeddedShaders);
        }

        void Renderer::Release()
        {
            delete s_Instance;

            s_Instance = nullptr;
        }

        void Renderer::LoadEngineShaders(bool loadEmbeddedShaders)
        {

        }

        /*void Renderer::DrawMesh(CommandBuffer* commandBuffer, Graphics::Pipeline* pipeline, Graphics::Mesh* mesh)
        {
            mesh->GetVertexBuffer()->Bind(commandBuffer, pipeline);
            mesh->GetIndexBuffer()->Bind(commandBuffer);
            Renderer::DrawIndexed(commandBuffer, DrawType::TRIANGLE, mesh->GetIndexBuffer()->GetCount());
            mesh->GetVertexBuffer()->Unbind();
            mesh->GetIndexBuffer()->Unbind();
        }*/
	}
}