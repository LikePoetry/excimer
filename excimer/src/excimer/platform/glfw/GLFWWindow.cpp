#include "hzpch.h"

#include "GLFWWindow.h"

#include "excimer/graphics/rhi/GraphicsContext.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Excimer
{
	static bool s_GLFWInitialized = false;
	static int s_NumGLFWWindows = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		EXCIMER_LOG_ERROR("GLFW Error - {0} : {1}", error, description);
	}

	GLFWWindow::GLFWWindow(const WindowDesc& properties)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_Init = false;
		m_VSync = properties.VSync;

		EXCIMER_LOG_INFO("VSync : {0}", m_VSync ? "True" : "False");
		m_HasResized = true;
		m_Data.m_RenderAPI = static_cast<Graphics::RenderAPI>(properties.RenderAPI);
		m_Data.VSync = m_VSync;
		m_Init = Init(properties);


	}

	GLFWWindow::~GLFWWindow()
	{

	}

	bool GLFWWindow::Init(const WindowDesc& properties)
	{
		EXCIMER_PROFILE_FUNCTION();
		EXCIMER_LOG_INFO("Creating window - Title : {0}, Width : {1}, Height : {2}", properties.Title, properties.Width, properties.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			EXCIMER_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		float xscale, yscale;
		glfwGetMonitorContentScale(monitor, &xscale, &yscale);
		m_Data.DPIScale = xscale;

		if (m_Data.m_RenderAPI == Graphics::RenderAPI::OPENGL)
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		SetBorderlessWindow(properties.Borderless);

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;

		if (properties.Fullscreen)
		{
			ScreenWidth = mode->width;
			ScreenHeight = mode->height;
		}
		else
		{
			ScreenWidth = properties.Width;
			ScreenHeight = properties.Height;
		}

		m_Data.Title = properties.Title;
		m_Data.Width = ScreenWidth;
		m_Data.Height = ScreenHeight;
		m_Data.Exit = false;

		if (m_Data.m_RenderAPI == Graphics::RenderAPI::VULKAN)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		m_Handle = glfwCreateWindow(ScreenWidth, ScreenHeight, properties.Title.c_str(), nullptr, nullptr);

		int w, h;
		glfwGetFramebufferSize(m_Handle, &w, &h);
		m_Data.Width = w;
		m_Data.Height = h;

		//TODO 是否引入对OpenGL的支持，商榷! 
		//if(m_Data.m_RenderAPI==Graphics::RenderAPI::OPENGL)
		//{
		//	glfwMakeContextCurrent(m_Handle);

		//	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
		//	{
		//		EXCIMER_LOG_ERROR("Failed to initialise OpenGL context");
		//	}
		//}

		glfwSetWindowUserPointer(m_Handle, &m_Data);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(m_Handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		SetIcon("//Textures/icon.png", "//Textures/icon32.png");
	}

	void GLFWWindow::MakeDefault()
	{
		CreateFunc = CreateFuncGLFW;
	}

	Window* GLFWWindow::CreateFuncGLFW(const WindowDesc& properties)
	{
		return new GLFWWindow(properties);
	}

	void GLFWWindow::SetBorderlessWindow(bool borderless)
	{
		EXCIMER_PROFILE_FUNCTION();
		if (borderless)
		{
			glfwWindowHint(GLFW_DECORATED, false);
		}
		else
		{
			glfwWindowHint(GLFW_DECORATED, true);
		}
	}

	void GLFWWindow::SetIcon(const std::string& file, const std::string& smallIconFilePath)
	{
		uint32_t width, height;
	}
}