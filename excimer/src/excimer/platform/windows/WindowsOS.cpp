#include "hzpch.h"
#include "WindowsOS.h"

#include "excimer/core/Application.h"
#include "excimer/platform/glfw/GLFWWindow.h"

extern Excimer::Application* Excimer::CreateApplication();

namespace Excimer
{
	void WindowsOS::Run()
	{
		EXCIMER_LOG_INFO("--------------------");
		EXCIMER_LOG_INFO(" System Information ");
		EXCIMER_LOG_INFO("--------------------");
		
		auto& app = Excimer::Application::Get();
		app.Init();
		app.Run();
		app.Release();
	}

	void WindowsOS::Init()
	{
		GLFWWindow::MakeDefault();
	}

	std::string WindowsOS::GetExecutablePath()
	{
		return "";
	}
}