#pragma once
#include "excimer/core/ExLog.h"
#include "excimer/core/Core.h"
#include "excimer/core/Reference.h"

namespace Excimer 
{
	class Window;

	class EXCIMER_EXPORT Application
	{
		friend class Editor;

	public:
		Application();

		void Run();
		bool OnFrame();

		virtual void OnQuit();
		virtual void Init();


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
			int RenderAPI;
			int ProjectVersion;
			int8_t DesiredGPUIndex = -1;
		};

		ProjectSettings& GetProjectSettings() { return m_ProjectSettings; }

	protected:
		ProjectSettings m_ProjectSettings;
		bool m_ProjectLoaded = false;

	private:

		uint32_t m_Frames = 0;
		uint32_t m_Updates = 0;

		uint32_t m_SceneViewWidth = 0;
		uint32_t m_SceneViewHeight = 0;

		UniquePtr<Window> m_Window;

		static Application* s_Instance;

		NONCOPYABLE(Application)
	};

	// 在客户端中定义
	Application* CreateApplication();
}