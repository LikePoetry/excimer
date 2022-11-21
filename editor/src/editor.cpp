#include "Editor.h"
#include "excimer/core/os/Window.h"
#include "excimer/core/Profiler.h"
#include "excimer/core/os/os.h"
#include "excimer/core/os/FileSystem.h"
#include "excimer/core/StringUtilities.h"
#include "EditorSettingsPanel.h"

namespace Excimer
{
	Editor::Editor() :Application()
	{

	}

	Editor::~Editor()
	{
	}

	void Editor::Init()
	{
		//������Ŀ�ļ�·��
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

		m_Panels.emplace_back(CreateSharedPtr<EditorSettingsPanel>());

		for (auto& panel : m_Panels)
			panel->SetEditor(this);

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
	// ���Ʋ˵���
	//==========================
	void Editor::DrawMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			// �ļ��˵�
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
			ImGui::EndMainMenuBar();
		}
	}

	//===========================
	// ����Dock ����
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
}