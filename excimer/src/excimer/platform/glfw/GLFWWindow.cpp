#include "hzpch.h"

#include "GLFWWindow.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Excimer
{
    void GLFWWindow::MakeDefault()
    {
        CreateFunc = CreateFuncGLFW;
    }

    Window* GLFWWindow::CreateFuncGLFW(const WindowDesc& properties)
    {
        return new GLFWWindow(properties);
    }
}