#include "hzpch.h"
#include "WindowsOS.h"

#include "excimer/core/Application.h"
#include "excimer/platform/glfw/GLFWWindow.h"

extern Excimer::Application* Excimer::CreateApplication();

namespace Excimer
{
	void WindowsOS::Run()
	{

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