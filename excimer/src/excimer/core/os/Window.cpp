#include "hzpch.h"
#include "Window.h"

namespace Excimer
{
	Window* (*Window::CreateFunc)(const WindowDesc&) = NULL;

	Window::~Window()
	{

	}
}