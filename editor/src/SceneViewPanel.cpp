#include "SceneViewPanel.h"

#include <excimer/imgui/IconsMaterialDesignIcons.h>

namespace Excimer
{
	SceneViewPanel::SceneViewPanel()
	{
		m_Name= ICON_MDI_GAMEPAD_VARIANT " Scene###scene";
		m_SimpleName = "Scene";
		m_CurrentScene = nullptr;

		m_Width = 1280;
		m_Height = 800;

	}

	void SceneViewPanel::OnImGui()
	{
		EXCIMER_PROFILE_FUNCTION();
		Application& app = Application::Get();

		ImGuiUtilities::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		if (!ImGui::Begin(m_Name.c_str(), &m_Active, flags) || !m_CurrentScene)
		{
			app.SetDisableMainRenderGraph(true);
			ImGui::End();
			return;
		}

		Camera* camera = nullptr;
		Maths::Transform* transform = nullptr;

		app.SetDisableMainRenderGraph(false);

		{
			EXCIMER_PROFILE_SCOPE("Set Override Camera");
			camera = m_Editor->GetCamera();
			transform = &m_Editor->GetEditorCameraTransform();

			app.GetRenderGraph()->SetOverrideCamera(camera, transform);
		}

		ImGui::End();
	}


	void SceneViewPanel::OnNewScene(Scene* scene)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_Editor->GetSettings().m_AspectRatio = 1.0f;
		m_CurrentScene = scene;

		auto renderGraph = Application::Get().GetRenderGraph();

		renderGraph->SetOverrideCamera(m_Editor->GetCamera(), &m_Editor->GetEditorCameraTransform());

	}
}