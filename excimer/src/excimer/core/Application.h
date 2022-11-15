#pragma once
#include "excimer/core/ExLog.h"
#include "excimer/core/Core.h"
#include "excimer/core/Reference.h"
#include "excimer/events/ApplicationEvent.h"

#include "excimer/utilities/AssetManager.h"

namespace Excimer
{
	class Window;
	struct WindowDesc;
	class Editor;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;

	enum class AppState
	{
		Running,
		Loading,
		Closing
	};

	enum class EditorState
	{
		Paused,
		Play,
		Next,
		Preview
	};

	enum class AppType
	{
		Game,
		Editor
	};

	class EXCIMER_EXPORT Application
	{
		friend class Editor;

	public:
		Application();

		void Run();
		bool OnFrame();

		virtual void OnQuit();
		virtual void Init();
		virtual void OnEvent(Event& e);

		Window* GetWindow() const
		{
			return m_Window.get();
		}

		SharedPtr<ShaderLibrary>& GetShaderLibrary();

		static Application& Get()
		{
			return *s_Instance;
		}

		static void Release()
		{
			if (s_Instance)
				delete s_Instance;
			s_Instance = nullptr;
		}

		bool OnWindowResize(WindowResizeEvent& e);

		struct ProjectSettings
		{
			std::string m_ProjectRoot;
			std::string m_ProjectName;
			std::string m_EngineAssetPath;
			uint32_t Width = 800, Height = 600;
			bool Fullscreen = false;
			bool VSync = true;
			bool Borderless = false;
			bool ShowConsole = true;
			std::string Title;
			int RenderAPI = 1;
			int ProjectVersion;
			int8_t DesiredGPUIndex = -1;
		};

		ProjectSettings& GetProjectSettings() { return m_ProjectSettings; }

	protected:
		ProjectSettings m_ProjectSettings;
		bool m_ProjectLoaded = false;

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		uint32_t m_Frames = 0;
		uint32_t m_Updates = 0;

		uint32_t m_SceneViewWidth = 0;
		uint32_t m_SceneViewHeight = 0;

		UniquePtr<Window> m_Window;

		SharedPtr<ShaderLibrary> m_ShaderLibrary;

		AppState m_CurrentState = AppState::Loading;
		EditorState m_EditorState = EditorState::Preview;
		AppType m_AppType = AppType::Editor;

		static Application* s_Instance;

		NONCOPYABLE(Application)
	};

	// 在客户端中定义
	Application* CreateApplication();
}