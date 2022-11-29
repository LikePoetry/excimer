#include "Editor.h"
#include "excimer/core/os/Window.h"
#include "excimer/core/Profiler.h"
#include "excimer/core/os/os.h"
#include "excimer/core/os/FileSystem.h"
#include "excimer/core/StringUtilities.h"
#include "excimer/core/os/Input.h"
#include "excimer/scene/Entity.h"
#include "excimer/scene/component/ModelComponent.h"
#include "excimer/scene/component/Components.h"

#include "excimer/graphics/Sprite.h"
#include "excimer/graphics/AnimatedSprite.h"

#include "excimer/graphics/Light.h"

#include "excimer/graphics/renderers/DebugRenderer.h"

#include "EditorSettingsPanel.h"
#include "SceneViewPanel.h"
#include "InspectorPanel.h"

namespace Excimer
{
	Editor* Editor::s_Editor = nullptr;

	Editor::Editor() :Application()
	{
		s_Editor = this;
	}

	Editor::~Editor()
	{
	}

	void Editor::Init()
	{
		//设置项目文件路径
		std::vector<std::string> projectLocations = {
			"ExampleProject/Example.lmproj",
			"/Users/jmorton/dev/Slight/ExampleProject/Example.lmproj",
			"../ExampleProject/Example.lmproj",
			OS::Instance()->GetExecutablePath() + "/ExampleProject/Example.lmproj",
			OS::Instance()->GetExecutablePath() + "/../ExampleProject/Example.lmproj",
			OS::Instance()->GetExecutablePath() + "/../../ExampleProject/Example.lmproj"
		};

		bool fileFound = false;
		std::string filePath;
		for (auto& path : projectLocations)
		{
			if (FileSystem::FileExists(path))
			{
				EXCIMER_LOG_INFO("Loaded Project {0}", path);
				m_ProjectSettings.m_ProjectRoot = StringUtilities::GetFileLocation(path);
				m_ProjectSettings.m_ProjectName = "Example";
				break;
			}
		}

		Application::Init();

		Application::Get().GetWindow()->SetEventCallback(BIND_EVENT_FN(Editor::OnEvent));

		//设置场景中的相机
		m_EditorCamera = CreateSharedPtr<Camera>(0.0f,
			0.0f,
			glm::vec3(0.0f, 0.0f, 50.0f),
			60.0f,
			0.1f,
			1000.0f,
			(float)Application::Get().GetWindowSize().x / (float)Application::Get().GetWindowSize().y);

		m_CurrentCamera = m_EditorCamera.get();
		glm::mat4 viewMat = glm::inverse(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		m_EditorCameraTransform.SetLocalTransform(viewMat);

		//设置组件小图标
		m_ComponentIconMap[typeid(Maths::Transform).hash_code()] = ICON_MDI_VECTOR_LINE;
		m_ComponentIconMap[typeid(Graphics::Light).hash_code()] = ICON_MDI_LIGHTBULB;
		m_ComponentIconMap[typeid(Graphics::ModelComponent).hash_code()] = ICON_MDI_SHAPE;

		m_Panels.emplace_back(CreateSharedPtr<EditorSettingsPanel>());
		m_Panels.emplace_back(CreateSharedPtr<SceneViewPanel>());
		m_Panels.emplace_back(CreateSharedPtr<InspectorPanel>());



		for (auto& panel : m_Panels)
			panel->SetEditor(this);

		//设置ImGui样式;
		ImGuiUtilities::SetTheme(m_Settings.m_Theme);
		OS::Instance()->SetTitleBarColour(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);

		//设置窗体名称
		Application::Get().GetWindow()->SetWindowTitle("Excimer Editor");

	}

	void Editor::OnEvent(Event& e)
	{
		EXCIMER_PROFILE_FUNCTION();
		// TODO

		Application::OnEvent(e);
	}

	void Editor::OnImGui()
	{
		EXCIMER_PROFILE_FUNCTION();
		DrawMenuBar();

		BeginDockSpace(m_Settings.m_FullScreenOnPlay && Application::Get().GetEditorState() == EditorState::Play);

		for (auto& panel : m_Panels)
		{
			if (panel->Active())
				panel->OnImGui();
		}
		//bool showDemo = true;
		//ImGui::ShowDemoWindow(&showDemo);

		EndDockSpace();

		Application::OnImGui();
	}

	//==========================
	// 绘制菜单栏
	//==========================
	void Editor::DrawMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// 文件菜单
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Project"))
				{

				}
				ImGui::Separator();
				if (ImGui::MenuItem("Open File"))
				{

				}


				ImGui::EndMenu();
			}

			// 添加实体
			if (ImGui::BeginMenu("Entity"))
			{
				auto scene = Application::Get().GetSceneManager()->GetCurrentScene();

				if (ImGui::MenuItem("Cube"))
				{
					auto entity = scene->CreateEntity("Cube");
					entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Cube);
					entity.AddComponent<Graphics::Light>();
				}

				if (ImGui::MenuItem("Sphere"))
				{
					auto entity = scene->CreateEntity("Sphere");
					entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Sphere);
					entity.GetComponent<Graphics::ModelComponent>().ModelRef.get()->GetMeshes().clear();
					entity.GetComponent<Graphics::ModelComponent>().ModelRef.get()->GetMeshes()
						.push_back(Excimer::SharedPtr<Excimer::Graphics::Mesh>(Excimer::Graphics::CreateSphere(64, 64)));

					entity.AddComponent<Graphics::Light>();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGuiUtilities::Property("Colour", colourProperty, 0.0f, 1.0f, false, Excimer::ImGuiUtilities::PropertyFlag::ColourProperty);
		ImGuiUtilities::Property("Thickness", thickness, Excimer::ImGuiUtilities::PropertyFlag::None);

		glm::vec3 test = m_EditorCameraTransform.GetWorldPosition();

		ImGuiUtilities::Property("CameraWorldPosition", test, Excimer::ImGuiUtilities::PropertyFlag::None);
	}

	//===========================
	// 处理Dock 窗体
	//===========================
	void Editor::BeginDockSpace(bool gameFullScreen)
	{
		EXCIMER_PROFILE_FUNCTION();
		static bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			auto pos = viewport->Pos;
			auto size = viewport->Size;
			bool menuBar = true;
			if (menuBar)
			{
				const float infoBarSize = ImGui::GetFrameHeight();
				pos.y += infoBarSize;
				size.y -= infoBarSize;
			}

			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the
		// pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_DockSpace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("MyDockspace", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiID DockspaceID = ImGui::GetID("MyDockspace");

		static std::vector<SharedPtr<EditorPanel>> hiddenPanels;
		if (m_Settings.m_FullScreenSceneView != gameFullScreen)
		{
			m_Settings.m_FullScreenSceneView = gameFullScreen;

			if (m_Settings.m_FullScreenSceneView)
			{
				for (auto panel : m_Panels)
				{
					if (panel->GetSimpleName() != "Game" && panel->Active())
					{
						panel->SetActive(false);
						hiddenPanels.push_back(panel);
					}
				}
			}
			else
			{
				for (auto panel : hiddenPanels)
				{
					panel->SetActive(true);
				}

				hiddenPanels.clear();
			}
		}

		if (!ImGui::DockBuilderGetNode(DockspaceID))
		{
			ImGui::DockBuilderRemoveNode(DockspaceID); // Clear out existing layout
			ImGui::DockBuilderAddNode(DockspaceID); // Add empty node
			ImGui::DockBuilderSetNodeSize(DockspaceID, ImGui::GetIO().DisplaySize * ImGui::GetIO().DisplayFramebufferScale);

			ImGuiID dock_main_id = DockspaceID;
			ImGuiID DockBottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
			ImGuiID DockLeft = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
			ImGuiID DockRight = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);

			ImGuiID DockLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.875f, nullptr, &DockLeft);
			ImGuiID DockRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.875f, nullptr, &DockRight);
			ImGuiID DockingLeftDownChild = ImGui::DockBuilderSplitNode(DockLeftChild, ImGuiDir_Down, 0.06f, nullptr, &DockLeftChild);
			ImGuiID DockingRightDownChild = ImGui::DockBuilderSplitNode(DockRightChild, ImGuiDir_Down, 0.06f, nullptr, &DockRightChild);

			ImGuiID DockBottomChild = ImGui::DockBuilderSplitNode(DockBottom, ImGuiDir_Down, 0.2f, nullptr, &DockBottom);
			ImGuiID DockingBottomLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.4f, nullptr, &DockLeft);
			ImGuiID DockingBottomRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.4f, nullptr, &DockRight);

			ImGuiID DockMiddle = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.8f, nullptr, &dock_main_id);
			ImGuiID DockBottomMiddle = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Down, 0.3f, nullptr, &DockMiddle);
			ImGuiID DockMiddleLeft = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Left, 0.5f, nullptr, &DockMiddle);
			ImGuiID DockMiddleRight = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Right, 0.5f, nullptr, &DockMiddle);

			ImGui::DockBuilderDockWindow("###game", DockMiddleRight);
			ImGui::DockBuilderDockWindow("###scene", DockMiddleLeft);
			ImGui::DockBuilderDockWindow("###inspector", DockRight);
			ImGui::DockBuilderDockWindow("###console", DockBottomMiddle);
			ImGui::DockBuilderDockWindow("###profiler", DockingBottomLeftChild);
			ImGui::DockBuilderDockWindow("###resources", DockingBottomLeftChild);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", DockLeft);
			ImGui::DockBuilderDockWindow("GraphicsInfo", DockLeft);
			ImGui::DockBuilderDockWindow("ApplicationInfo", DockLeft);
			ImGui::DockBuilderDockWindow("###hierarchy", DockLeft);
			ImGui::DockBuilderDockWindow("###textEdit", DockMiddle);
			ImGui::DockBuilderDockWindow("###scenesettings", DockLeft);

			ImGui::DockBuilderFinish(DockspaceID);
		}

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::DockSpace(DockspaceID, ImVec2(0.0f, 0.0f), opt_flags);
		}

	}

	void Editor::EndDockSpace()
	{
		ImGui::End();
	}

	void Editor::SaveEditorSettings()
	{

	}

	void Editor::OnNewScene(Scene* scene)
	{
		EXCIMER_PROFILE_FUNCTION();
		Application::OnNewScene(scene);
		m_SelectedEntity = entt::null;

		glm::mat4 viewMat = glm::inverse(glm::lookAt(glm::vec3(-31.0f, 12.0f, 51.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		m_EditorCameraTransform.SetLocalTransform(viewMat);

		for (auto panel : m_Panels)
		{
			panel->OnNewScene(scene);
		}
	}

	void Editor::OnUpdate(const TimeStep& ts)
	{
		EXCIMER_PROFILE_FUNCTION();
		if (m_SceneViewActive)
		{
			auto& registry = Application::Get().GetSceneManager()->GetCurrentScene()->GetRegistry();

			{
				const glm::vec2 mousePos = Input::Get().GetMousePosition();
				m_EditorCameraController.SetCamera(*m_EditorCamera.get());
				m_EditorCameraController.HandleMouse(m_EditorCameraTransform, (float)ts.GetSeconds(), mousePos.x, mousePos.y);
				m_EditorCameraController.HandleKeyboard(m_EditorCameraTransform, (float)ts.GetSeconds());
			}
		}
		else
		{
			m_EditorCameraController.StopMovement();
		}

		Application::OnUpdate(ts);
	}

	Maths::Ray Editor::GetScreenRay(int x, int y, Camera* camera, int width, int height)
	{
		//根据点击点和相机视角发射射线
		EXCIMER_PROFILE_FUNCTION();
		if (!camera)
			return Maths::Ray();

		float screenX = (float)x / (float)width;
		float screenY = (float)y / (float)height;

		bool flipY = true;

		return camera->GetScreenRay(screenX, screenY, glm::inverse(m_EditorCameraTransform.GetWorldMatrix()), flipY);
	}

	void Editor::SelectObject(const Maths::Ray& ray)
	{
		//根据射线选中物体
		EXCIMER_PROFILE_FUNCTION();
		auto& registry = Application::Get().GetSceneManager()->GetCurrentScene()->GetRegistry();
		float closestEntityDist = Maths::M_INFINITY;
		entt::entity currentClosestEntity = entt::null;

		auto group = registry.group<Graphics::ModelComponent>(entt::get<Maths::Transform>);

		static Timer timer;
		static float timeSinceLastSelect = 0.0f;

		for (auto entity : group)
		{
			const auto& [model, trans] = group.get<Graphics::ModelComponent, Maths::Transform>(entity);

			auto& meshes = model.ModelRef->GetMeshes();

			for (auto mesh : meshes)
			{
				if (mesh->GetActive())
				{
					auto& worldTransform = trans.GetWorldMatrix();

					auto bbCopy = mesh->GetBoundingBox()->Transformed(worldTransform);
					float distance;
					ray.Intersects(bbCopy, distance);

					if (distance < Maths::M_INFINITY)
					{
						if (distance < closestEntityDist)
						{
							closestEntityDist = distance;
							currentClosestEntity = entity;
						}
					}
				}
			}
		}

		if (m_SelectedEntity != entt::null)
		{
			if (m_SelectedEntity == currentClosestEntity)
			{
				if (timer.GetElapsedS() - timeSinceLastSelect < 1.0f)
				{
					auto& trans = registry.get<Maths::Transform>(m_SelectedEntity);
					auto& model = registry.get<Graphics::ModelComponent>(m_SelectedEntity);
					auto bb = model.ModelRef->GetMeshes().front()->GetBoundingBox()->Transformed(trans.GetWorldMatrix());

					//移动相机到物体
					//FocusCamera(trans.GetWorldPosition(), glm::distance(bb.Max(), bb.Min()));
				}
				else
				{
					currentClosestEntity = entt::null;
				}
			}

			timeSinceLastSelect = timer.GetElapsedS();
			m_SelectedEntity = currentClosestEntity;
			return;
		}

		auto spriteGroup = registry.group<Graphics::Sprite>(entt::get<Maths::Transform>);

		for (auto entity : spriteGroup)
		{
			const auto& [sprite, trans] = spriteGroup.get<Graphics::Sprite, Maths::Transform>(entity);

			auto& worldTransform = trans.GetWorldMatrix();
			auto bb = Maths::BoundingBox(Maths::Rect(sprite.GetPosition(), sprite.GetPosition() + sprite.GetScale()));
			bb.Transform(trans.GetWorldMatrix());

			float distance;
			ray.Intersects(bb, distance);
			if (distance < Maths::M_INFINITY)
			{
				if (distance < closestEntityDist)
				{
					closestEntityDist = distance;
					currentClosestEntity = entity;
				}
			}
		}

		auto animSpriteGroup = registry.group<Graphics::AnimatedSprite>(entt::get<Maths::Transform>);

		for (auto entity : animSpriteGroup)
		{
			const auto& [sprite, trans] = animSpriteGroup.get<Graphics::AnimatedSprite, Maths::Transform>(entity);

			auto& worldTransform = trans.GetWorldMatrix();
			auto bb = Maths::BoundingBox(Maths::Rect(sprite.GetPosition(), sprite.GetPosition() + sprite.GetScale()));
			bb.Transform(trans.GetWorldMatrix());
			float distance;
			ray.Intersects(bb, distance);
			if (distance < Maths::M_INFINITY)
			{
				if (distance < closestEntityDist)
				{
					closestEntityDist = distance;
					currentClosestEntity = entity;
				}
			}
		}

		if (m_SelectedEntity != entt::null)
		{
			if (m_SelectedEntity == currentClosestEntity)
			{
				auto& trans = registry.get<Maths::Transform>(m_SelectedEntity);
				auto& sprite = registry.get<Graphics::Sprite>(m_SelectedEntity);
				auto bb = Maths::BoundingBox(Maths::Rect(sprite.GetPosition(), sprite.GetPosition() + sprite.GetScale()));

				//移动相机到物体
				//FocusCamera(trans.GetWorldPosition(), glm::distance(bb.Max(), bb.Min()));
			}
		}

		m_SelectedEntity = currentClosestEntity;
	}

	void Editor::OnRender()
	{
		DebugRenderer::DrawHairLine(glm::vec3(-5000.0f, 0.0f, 0.0f), glm::vec3(5000.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));// X轴
		DebugRenderer::DrawHairLine(glm::vec3(0.0f, -5000.0f, 0.0f), glm::vec3(0.0f, 5000.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));// Y轴
		DebugRenderer::DrawHairLine(glm::vec3(0.0f, 0.0f, -5000.0f), glm::vec3(0.0f, 0.0f, 5000.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));// Z轴

		DebugRenderer::DrawPoint(pointTips, thickness, colourProperty);
		DebugRenderer::DrawHairLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), colourProperty);
		DebugRenderer::DrawThickLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 100.0f, 0.0f), thickness, colourProperty);
		DebugRenderer::DrawTriangle(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(30.0f, 0.0f, 0.0f), glm::vec3(0.0f, 30.0f, 0.0f), colourProperty);
		Application::OnRender();
	}
}