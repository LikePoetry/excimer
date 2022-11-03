#include "hzpch.h"
#include "Application.h"
#include "Profiler.h"
#include "excimer/core/os/Window.h"



namespace Excimer {

	Application* Application::s_Instance = nullptr;

	Application::Application()
		:m_Frames(0)
		,m_Updates(0)
		,m_SceneViewWidth(800)
		,m_SceneViewHeight(600)
	{
		EXCIMER_PROFILE_FUNCTION();
		EXCIMER_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
	}

	void Application::Init()
	{

		WindowDesc windowDesc;
		windowDesc.Width = m_ProjectSettings.Width;
		windowDesc.Height = m_ProjectSettings.Height;
		windowDesc.RenderAPI = m_ProjectSettings.RenderAPI;
		windowDesc.Fullscreen = m_ProjectSettings.Fullscreen;
		windowDesc.Borderless = m_ProjectSettings.Borderless;
		windowDesc.ShowConsole = m_ProjectSettings.ShowConsole;
		windowDesc.Title = m_ProjectSettings.Title;
		windowDesc.VSync = m_ProjectSettings.VSync;

		// Initialise the Window
		m_Window = UniquePtr<Window>(Window::Create(windowDesc));
		if (!m_Window->HasInitialised())
			OnQuit();

		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	}

	void Application::OnEvent(Event& e)
	{
		EXCIMER_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));



	}

	void Application::OnQuit()
	{

	}

	bool Application::OnFrame()
	{
		m_Window->ProcessInput();

		if (m_CurrentState == AppState::Closing)
			return false;
		return true;
	}

	void Application::Run()
	{
		while (OnFrame())
		{
		}

		//OnQuit();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_CurrentState = AppState::Closing;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		return true;
	}


}
