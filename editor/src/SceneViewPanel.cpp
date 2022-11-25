#include "SceneViewPanel.h"

#include <excimer/imgui/IconsMaterialDesignIcons.h>
#include <excimer/maths/Maths.h>
#include <excimer/core/os/Input.h>
#include <excimer/graphics/camera/Camera.h>

namespace Excimer
{
	SceneViewPanel::SceneViewPanel()
	{
		m_Name = ICON_MDI_GAMEPAD_VARIANT " Scene###scene";
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

		ImVec2 offset = { 0.0f, 0.0f };

		{
			//TODO ����Toolbar();

			ToolBar();

			offset = ImGui::GetCursorPos(); // Usually ImVec2(0.0f, 50.0f);
		}

		if (!camera)
		{
			ImGui::End();
			ImGui::PopStyleVar();
			return;
		}

		auto sceneViewSize = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() - offset * 0.5f; // - offset * 0.5f;
		auto sceneViewPosition = ImGui::GetWindowPos() + offset;

		sceneViewSize.x -= static_cast<int>(sceneViewSize.x) % 2 != 0 ? 1.0f : 0.0f;
		sceneViewSize.y -= static_cast<int>(sceneViewSize.y) % 2 != 0 ? 1.0f : 0.0f;

		float aspect = static_cast<float>(sceneViewSize.x) / static_cast<float>(sceneViewSize.y);

		if (!Maths::Equals(aspect, camera->GetAspectRatio()))
		{
			camera->SetAspectRatio(aspect);
		}
		m_Editor->m_SceneViewPanelPosition = glm::vec2(sceneViewPosition.x, sceneViewPosition.y);

		if (m_Editor->GetSettings().m_HalfRes)
			sceneViewSize *= 0.5f;

		Resize(static_cast<uint32_t>(sceneViewSize.x), static_cast<uint32_t>(sceneViewSize.y));

		if (m_Editor->GetSettings().m_HalfRes)
			sceneViewSize *= 2.0f;

		ImGuiUtilities::Image(m_GameViewTexture.get(), glm::vec2(sceneViewSize.x, sceneViewSize.y));


		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = sceneViewPosition;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		bool updateCamera = ImGui::IsMouseHoveringRect(minBound, maxBound); // || Input::Get().GetMouseMode() == MouseMode::Captured;

		m_Editor->SetSceneViewActive(updateCamera);

		if (updateCamera && app.GetSceneActive() && Input::Get().GetMouseClicked(InputCode::MouseKey::ButtonLeft))
		{
			//�ڳ��������ѡ������
			EXCIMER_PROFILE_SCOPE("Select Object");
			float dpi = Application::Get().GetWindowDPI();
			auto clickPos = Input::Get().GetMousePosition() - glm::vec2(sceneViewPosition.x / dpi, sceneViewPosition.y / dpi);

			Maths::Ray ray = m_Editor->GetScreenRay(int(clickPos.x), int(clickPos.y), camera, int(sceneViewSize.x / dpi), int(sceneViewSize.y / dpi));
			m_Editor->SelectObject(ray);
		}

		ImGui::End();
	}

	void SceneViewPanel::ToolBar()
	{
		EXCIMER_PROFILE_FUNCTION();

		auto& camera = *m_Editor->GetCamera();

		ImGui::Indent();
		if (ImGui::Button(ICON_MDI_ANGLE_RIGHT "2D"))
		{
			//�����ӽ�
			//camera.SetIsOrthographic(true);
			m_Editor->GetEditorCameraTransform().SetLocalOrientation(glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)));
		}
		ImGui::Unindent();
	}


	void SceneViewPanel::OnNewScene(Scene* scene)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_Editor->GetSettings().m_AspectRatio = 1.0f;
		m_CurrentScene = scene;

		auto renderGraph = Application::Get().GetRenderGraph();
		renderGraph->SetRenderTarget(m_GameViewTexture.get(), true);
		renderGraph->SetOverrideCamera(m_Editor->GetCamera(), &m_Editor->GetEditorCameraTransform());

	}

	void SceneViewPanel::Resize(uint32_t width, uint32_t height)
	{
		EXCIMER_PROFILE_FUNCTION();
		bool resize = false;

		EXCIMER_ASSERT(width > 0 && height > 0, "Scene View Dimensions 0");

		Application::Get().SetSceneViewDimensions(width, height);

		if (m_Width != width || m_Height != height)
		{
			resize = true;
			m_Width = width;
			m_Height = height;
		}

		if (!m_GameViewTexture)
		{
			Graphics::TextureDesc mainRenderTargetDesc;
			mainRenderTargetDesc.format = Graphics::RHIFormat::R8G8B8A8_Unorm;
			mainRenderTargetDesc.flags = Graphics::TextureFlags::Texture_RenderTarget;

			m_GameViewTexture = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::Create(mainRenderTargetDesc, m_Width, m_Height));
		}

		if (resize)
		{
			m_GameViewTexture->Resize(m_Width, m_Height);

			auto renderGraph = Application::Get().GetRenderGraph();
			renderGraph->SetRenderTarget(m_GameViewTexture.get(), true, false);

			WindowResizeEvent e(width, height);
			auto& app = Application::Get();
			app.GetRenderGraph()->OnResize(width, height);

			renderGraph->OnEvent(e);
		}
	}
}