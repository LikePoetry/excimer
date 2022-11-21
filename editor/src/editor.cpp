#include "Editor.h"
#include "excimer/core/os/Window.h"
#include "excimer/core/Profiler.h"
#include "excimer/core/os/os.h"
#include "excimer/core/os/FileSystem.h"
#include "excimer/core/StringUtilities.h"

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
			ImGui::EndMainMenuBar();
		}
	}
}