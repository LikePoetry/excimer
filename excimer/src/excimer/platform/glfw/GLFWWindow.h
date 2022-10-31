#pragma once
#include "excimer/core/os/Window.h"

struct GLFWwindow;

namespace Excimer
{
	class EXCIMER_EXPORT GLFWWindow:public Window
	{
	public:
		GLFWWindow(const WindowDesc& properties);
		~GLFWWindow();
		static void MakeDefault();
	
	protected:
		static Window* CreateFuncGLFW(const WindowDesc& properties);
	};
}