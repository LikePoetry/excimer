#pragma once
#include "excimer/core/ExLog.h"
#include "excimer/core/Core.h"
#include "excimer/core/Reference.h"
#include "excimer/events/ApplicationEvent.h"
#include "excimer/utilities/Timer.h"
#include "excimer/utilities/AssetManager.h"
#include "excimer/scene/SceneManager.h"
#include "excimer/scene/Scene.h"
#include "excimer/scene/SystemManager.h"
#include "excimer/graphics/renderers/RenderGraph.h"
#include "excimer/imgui/ImGuiManager.h"

#include <cereal/types/vector.hpp>
#include <cereal/cereal.hpp>

namespace Excimer
{
	class Window;
	struct WindowDesc;
	class Editor;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;

	namespace Graphics
	{
		class RenderGraph;
		enum class RenderAPI : uint32_t;
	}

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

		void OnExitScene();

		virtual void OnQuit();
		virtual void Init();
		virtual void OnEvent(Event& e);
		virtual void OnNewScene(Scene* scene);
		virtual void OnImGui();

		Graphics::RenderGraph* GetRenderGraph() const
		{
			return m_RenderGraph.get();
		}

		Window* GetWindow() const
		{
			return m_Window.get();
		}

		AppState GetState() const
		{
			return m_CurrentState;
		}

		EditorState GetEditorState() const
		{
			return m_EditorState;
		}

		void SetSceneActive(bool active)
		{
			m_SceneActive = active;
		}

		bool GetSceneActive() const
		{
			return m_SceneActive;
		}

		Scene* GetCurrentScene() const;

		glm::vec2 GetWindowSize() const;
		float GetWindowDPI() const;

		SharedPtr<ShaderLibrary>& GetShaderLibrary();
		SharedPtr<ModelLibrary>& GetModelLibrary();
		SharedPtr<FontLibrary>& GetFontLibrary();

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

		virtual void Serialise();
		virtual void Deserialise();

		template <typename Archive>
		void save(Archive& archive) const

		{
			int projectVersion = 8;

			archive(cereal::make_nvp("Project Version", projectVersion));

			// Version 1

			std::string path;

			// Window size and full screen shouldnt be in project

			// Version 8 removed width and height
			archive(cereal::make_nvp("RenderAPI", m_ProjectSettings.RenderAPI),
				cereal::make_nvp("Fullscreen", m_ProjectSettings.Fullscreen),
				cereal::make_nvp("VSync", m_ProjectSettings.VSync),
				cereal::make_nvp("ShowConsole", m_ProjectSettings.ShowConsole),
				cereal::make_nvp("Title", m_ProjectSettings.Title));
			// Version 2

			auto paths = m_SceneManager->GetSceneFilePaths();
			std::vector<std::string> newPaths;
			for (auto& path : paths)
			{
				std::string newPath;
				VFS::Get().AbsoulePathToVFS(path, newPath);
				newPaths.push_back(path);
			}
			archive(cereal::make_nvp("Scenes", newPaths));
			// Version 3
			archive(cereal::make_nvp("SceneIndex", m_SceneManager->GetCurrentSceneIndex()));
			// Version 4
			archive(cereal::make_nvp("Borderless", m_ProjectSettings.Borderless));
			// Version 5
			archive(cereal::make_nvp("EngineAssetPath", m_ProjectSettings.m_EngineAssetPath));
			// Version 6
			archive(cereal::make_nvp("GPUIndex", m_ProjectSettings.DesiredGPUIndex));
		}

		template <typename Archive>
		void load(Archive& archive)
		{
			int sceneIndex = 0;
			archive(cereal::make_nvp("Project Version", m_ProjectSettings.ProjectVersion));

			std::string test;
			if (m_ProjectSettings.ProjectVersion < 8)
			{
				archive(cereal::make_nvp("RenderAPI", m_ProjectSettings.RenderAPI),
					cereal::make_nvp("Width", m_ProjectSettings.Width),
					cereal::make_nvp("Height", m_ProjectSettings.Height),
					cereal::make_nvp("Fullscreen", m_ProjectSettings.Fullscreen),
					cereal::make_nvp("VSync", m_ProjectSettings.VSync),
					cereal::make_nvp("ShowConsole", m_ProjectSettings.ShowConsole),
					cereal::make_nvp("Title", m_ProjectSettings.Title));
			}
			else
			{
				archive(cereal::make_nvp("RenderAPI", m_ProjectSettings.RenderAPI),
					cereal::make_nvp("Fullscreen", m_ProjectSettings.Fullscreen),
					cereal::make_nvp("VSync", m_ProjectSettings.VSync),
					cereal::make_nvp("ShowConsole", m_ProjectSettings.ShowConsole),
					cereal::make_nvp("Title", m_ProjectSettings.Title));
			}
			if (m_ProjectSettings.ProjectVersion > 2)
			{
				std::vector<std::string> sceneFilePaths;
				archive(cereal::make_nvp("Scenes", sceneFilePaths));

				for (auto& filePath : sceneFilePaths)
				{
					m_SceneManager->AddFileToLoadList(filePath);
				}

				if (m_SceneManager->GetScenes().size() == 0 && sceneFilePaths.size() == sceneIndex)
				{
					m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
					m_SceneManager->SwitchScene(0);
				}
			}
			if (m_ProjectSettings.ProjectVersion > 3)
			{
				archive(cereal::make_nvp("SceneIndex", sceneIndex));
				m_SceneManager->SwitchScene(sceneIndex);
			}
			if (m_ProjectSettings.ProjectVersion > 4)
			{
				archive(cereal::make_nvp("Borderless", m_ProjectSettings.Borderless));
			}

			if (m_ProjectSettings.ProjectVersion > 5)
			{
				archive(cereal::make_nvp("EngineAssetPath", m_ProjectSettings.m_EngineAssetPath));
			}
			else
				m_ProjectSettings.m_EngineAssetPath = "/Users/jmorton/dev/Excimer/Excimer/Assets/";

			if (m_ProjectSettings.ProjectVersion > 6)
				archive(cereal::make_nvp("GPUIndex", m_ProjectSettings.DesiredGPUIndex));

			VFS::Get().Mount("CoreShaders", m_ProjectSettings.m_EngineAssetPath + std::string("Shaders"));
		}

		void MountVFSPaths();

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

		bool m_SceneActive = true;

		uint32_t m_SceneViewWidth = 0;
		uint32_t m_SceneViewHeight = 0;

		UniquePtr<Window> m_Window;
		UniquePtr<SceneManager> m_SceneManager;
		UniquePtr<SystemManager> m_SystemManager;
		UniquePtr<Graphics::RenderGraph> m_RenderGraph;
		UniquePtr<ImGuiManager> m_ImGuiManager;
		UniquePtr<Timer> m_Timer;

		SharedPtr<ShaderLibrary> m_ShaderLibrary;
		SharedPtr<ModelLibrary> m_ModelLibrary;
		SharedPtr<FontLibrary> m_FontLibrary;

		AppState m_CurrentState = AppState::Loading;
		EditorState m_EditorState = EditorState::Preview;
		AppType m_AppType = AppType::Editor;

		static Application* s_Instance;

		NONCOPYABLE(Application)
	};

	// 在客户端中定义
	Application* CreateApplication();
}