#include "hzpch.h"

#include "GLFWWindow.h"

#include "excimer/core/Application.h"
#include "excimer/graphics/rhi/GraphicsContext.h"
#include "excimer/utilities/LoadImage.h"
#include "excimer/events/ApplicationEvent.h"
#include "excimer/events/KeyEvent.h"
#include "excimer/events/MouseEvent.h"
#include "excimer/core/ExLog.h"
#include "excimer/platform/glfw/GLFWKeyCodes.h"


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

		// Setting fullscreen overrides width and heigh in Init
		auto propCopy = properties;
		propCopy.Width = m_Data.Width;
		propCopy.Height = m_Data.Height;


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

		//SetIcon("//Textures/icon.png", "//Textures/icon32.png");

		// glfwSetWindowPos(m_Handle, mode->width / 2 - m_Data.Width / 2, mode->height / 2 - m_Data.Height / 2);
		glfwSetInputMode(m_Handle, GLFW_STICKY_KEYS, true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

				int w, h;
				glfwGetFramebufferSize(window, &w, &h);

				data.DPIScale = (float)w / (float)width;

				data.Width = uint32_t(width * data.DPIScale);
				data.Height = uint32_t(height * data.DPIScale);

				WindowResizeEvent event(data.Width, data.Height, data.DPIScale);
				data.EventCallback(event); 
			});

		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));
				WindowCloseEvent event;
				data.EventCallback(event);
				data.Exit = true; });

		glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* window, int focused)
			{
				Window* lmWindow = Application::Get().GetWindow();

				if (lmWindow)
					lmWindow->SetWindowFocus(focused); });

		glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* window, int32_t state)
			{
				switch (state)
				{
				case GL_TRUE:
					Application::Get().GetWindow()->SetWindowFocus(false);
					break;
				case GL_FALSE:
					Application::Get().GetWindow()->SetWindowFocus(true);
					break;
				default:
					EXCIMER_LOG_INFO("Unsupported window iconify state from callback");
				} });

		glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(GLFWKeyCodes::GLFWToExcimerKeyboardKey(key), 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(GLFWKeyCodes::GLFWToExcimerKeyboardKey(key));
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(GLFWKeyCodes::GLFWToExcimerKeyboardKey(key), 1);
					data.EventCallback(event);
					break;
				}
				} });

		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(GLFWKeyCodes::GLFWToExcimerMouseKey(button));
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(GLFWKeyCodes::GLFWToExcimerMouseKey(button));
					data.EventCallback(event);
					break;
				}
				} });

		glfwSetScrollCallback(m_Handle, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event); });

		glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));
				MouseMovedEvent event((float)xPos /* * data.DPIScale*/, (float)yPos /* * data.DPIScale*/);
				data.EventCallback(event); });

		glfwSetCursorEnterCallback(m_Handle, [](GLFWwindow* window, int enter)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

				MouseEnterEvent event(enter > 0);
				data.EventCallback(event); });

		glfwSetCharCallback(m_Handle, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

				KeyTypedEvent event(GLFWKeyCodes::GLFWToExcimerKeyboardKey(keycode), char(keycode));
				data.EventCallback(event); });

		glfwSetDropCallback(m_Handle, [](GLFWwindow* window, int numDropped, const char** filenames)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

				std::string filePath = filenames[0];
				WindowFileEvent event(filePath);
				data.EventCallback(event); });


		EXCIMER_LOG_INFO("Initialised GLFW version : {0}", glfwGetVersionString());
		return true;
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

	void GLFWWindow::ProcessInput()
	{
		EXCIMER_PROFILE_SCOPE("GLFW PollEvents");
		glfwPollEvents();

	}

	void GLFWWindow::SetIcon(const std::string& file, const std::string& smallIconFilePath)
	{
		uint32_t width, height;
		uint8_t* pixels = Excimer::LoadImageFromFile(file, &width, &height, nullptr, nullptr, true);

		if (!pixels)
		{
			EXCIMER_LOG_WARN("Failed to load app icon {0}", file);
			return;
		}

		std::vector<GLFWimage> images;
		GLFWimage image;
		image.height = height;
		image.width = width;
		image.pixels = static_cast<unsigned char*>(pixels);
		images.push_back(image);

		if (smallIconFilePath != "")
		{
			pixels = Excimer::LoadImageFromFile(smallIconFilePath, &width, &height, nullptr, nullptr, true);

			if (!pixels)
			{
				EXCIMER_LOG_WARN("Failed to load app icon {0}", smallIconFilePath);
				return;
			}

			image.height = height;
			image.width = width;
			image.pixels = static_cast<unsigned char*>(pixels);
			images.push_back(image);
		}

		glfwSetWindowIcon(m_Handle, int(images.size()), images.data());

		delete[] pixels;

	}
}