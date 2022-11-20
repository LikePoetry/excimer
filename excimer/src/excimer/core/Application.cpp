#include "hzpch.h"
#include "Application.h"
#include "Profiler.h"
#include "excimer/core/os/Window.h"
#include "excimer/core/os/FileSystem.h"
#include "excimer/core/Engine.h"
#include "excimer/core/StringUtilities.h"
#include "excimer/core/os/os.h"
#include "excimer/core/os/Input.h"

#include "excimer/graphics/rhi/Renderer.h"

#include "excimer/core/Profiler.h"

#include "excimer/embedded/splash.inl"

#include <filesystem>

#include <cereal/archives/json.hpp>
#include <imgui/imgui.h>

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
		EXCIMER_PROFILE_FUNCTION();
		//场景管理相关，配置初始化
		m_SceneManager = CreateUniquePtr<SceneManager>();
		Deserialise();

		Engine::Get();

		m_Timer = CreateUniquePtr<Timer>();

		//TODO 目前仅支持Vulkan API; 
		//m_ProjectSettings.RenderAPI 
		Graphics::GraphicsContext::SetRenderAPI(static_cast<Graphics::RenderAPI>(Excimer::Graphics::RenderAPI::VULKAN));

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

		m_EditorState = EditorState::Play;

		//初始化GUI
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		m_ShaderLibrary = CreateSharedPtr<ShaderLibrary>();
		m_ModelLibrary = CreateSharedPtr<ModelLibrary>();
		m_FontLibrary = CreateSharedPtr<FontLibrary>();

		//载入内置嵌入式着色器
		bool loadEmbeddedShaders = true;
		if (FileSystem::FolderExists(m_ProjectSettings.m_EngineAssetPath + "Shaders"))
			loadEmbeddedShaders = false;

		Graphics::Renderer::Init(loadEmbeddedShaders);

		//是否全屏显示
		if (m_ProjectSettings.Fullscreen)
			m_Window->Maximise();

		// Draw Splash Screeh 绘制启动画面
		{
			auto splashTexture = Graphics::Texture2D::CreateFromSource(splashWidth, splashHeight, (void*)splash);
			Graphics::Renderer::GetRenderer()->Begin();
			Graphics::Renderer::GetRenderer()->DrawSplashScreen(splashTexture);
			Graphics::Renderer::GetRenderer()->Present();
			// To Display the window
			m_Window->ProcessInput();
			m_Window->OnUpdate();

			delete splashTexture;
		}
	}

	void Application::OnEvent(Event& e)
	{
		EXCIMER_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));



	}

	void Application::OnNewScene(Scene* scene)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_RenderGraph->OnNewScene(scene);
	}

	void Application::OnQuit()
	{
		EXCIMER_PROFILE_FUNCTION();
		Serialise();

		Graphics::Material::ReleaseDefaultTexture();
		Graphics::Font::ShutdownDefaultFont();
		Engine::Release();
		Input::Release();

		m_ShaderLibrary.reset();
		m_ModelLibrary.reset();
		m_FontLibrary.reset();
		m_SceneManager.reset();
		m_RenderGraph.reset();
		m_SystemManager.reset();
		m_ImGuiManager.reset();

		Graphics::Pipeline::ClearCache();
		Graphics::RenderPass::ClearCache();
		Graphics::Framebuffer::ClearCache();
		Graphics::Renderer::Release();

		m_Window.reset();
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

	glm::vec2 Application::GetWindowSize() const
	{
		if (!m_Window)
			return glm::vec2(0.0f, 0.0f);
		return glm::vec2(static_cast<float>(m_Window->GetWidth()), static_cast<float>(m_Window->GetHeight()));
	}

	Scene* Application::GetCurrentScene() const
	{
		EXCIMER_PROFILE_FUNCTION();
		return m_SceneManager->GetCurrentScene();
	}

	float Application::GetWindowDPI() const
	{
		if (!m_Window)
			return 1.0f;

		return m_Window->GetDPIScale();
	}

	SharedPtr<ShaderLibrary>& Application::GetShaderLibrary()
	{
		return m_ShaderLibrary;
	}

	SharedPtr<ModelLibrary>& Application::GetModelLibrary()
	{
		return m_ModelLibrary;
	}

	SharedPtr<FontLibrary>& Application::GetFontLibrary()
	{
		return m_FontLibrary;
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

	void Application::OnImGui()
	{
		EXCIMER_PROFILE_FUNCTION();
		if (!m_SceneManager->GetCurrentScene())
			return;

		m_SceneManager->GetCurrentScene()->OnImGui();
	}

	void Application::OnExitScene()
	{
	}

	void Application::MountVFSPaths()
	{
		VFS::Get().Mount("Meshes", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Meshes"));
		VFS::Get().Mount("Textures", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Textures"));
		VFS::Get().Mount("Sounds", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Sounds"));
		VFS::Get().Mount("Scripts", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Scripts"));
		VFS::Get().Mount("Scenes", m_ProjectSettings.m_ProjectRoot + std::string("Assets/Scenes"));
		VFS::Get().Mount("Assets", m_ProjectSettings.m_ProjectRoot + std::string("Assets"));
	}

	void Application::Serialise()
	{
		EXCIMER_PROFILE_FUNCTION();
		{
			std::stringstream storage;
			{
				// output finishes flushing its contents when it goes out of scope
				cereal::JSONOutputArchive output{ storage };
				output(*this);
			}
			auto fullPath = m_ProjectSettings.m_ProjectRoot + m_ProjectSettings.m_ProjectName + std::string(".lmproj");
			EXCIMER_LOG_INFO("Serialising Application {0}", fullPath);
			FileSystem::WriteTextFile(fullPath, storage.str());
		}
	}

	void Application::Deserialise()
	{
		EXCIMER_PROFILE_FUNCTION();

		{
			auto filePath = m_ProjectSettings.m_ProjectRoot + m_ProjectSettings.m_ProjectName + std::string(".lmproj");

			MountVFSPaths();

			if(!FileSystem::FileExists(filePath))
			{
				EXCIMER_LOG_INFO("No saved Project file found {0}", filePath);

				{
					m_SceneManager = CreateUniquePtr<SceneManager>();

					// Set Default values
					m_ProjectSettings.RenderAPI = 1;
					m_ProjectSettings.Width = 1200;
					m_ProjectSettings.Height = 800;
					m_ProjectSettings.Borderless = false;
					m_ProjectSettings.VSync = true;
					m_ProjectSettings.Title = "App";
					m_ProjectSettings.ShowConsole = false;
					m_ProjectSettings.Fullscreen = false;

					m_ProjectLoaded = false;

					m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../Excimer/Assets/";

					VFS::Get().Mount("CoreShaders", m_ProjectSettings.m_EngineAssetPath + std::string("Shaders"));

					m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
					m_SceneManager->SwitchScene(0);
				}

				return;
			}

			// 存在项目文件的情况下，载入项目文件
			m_ProjectLoaded = true;
			std::string data = FileSystem::ReadTextFile(filePath);
			std::istringstream istr;
			istr.str(data);
			try
			{
				cereal::JSONInputArchive input(istr);
				input(*this);
			}
			catch (...)
			{
				// Set Default values
				m_ProjectSettings.RenderAPI = 1;
				m_ProjectSettings.Width = 1200;
				m_ProjectSettings.Height = 800;
				m_ProjectSettings.Borderless = false;
				m_ProjectSettings.VSync = true;
				m_ProjectSettings.Title = "App";
				m_ProjectSettings.ShowConsole = false;
				m_ProjectSettings.Fullscreen = false;

				m_ProjectSettings.m_EngineAssetPath = StringUtilities::GetFileLocation(OS::Instance()->GetExecutablePath()) + "../../Slight/Assets/";

				VFS::Get().Mount("CoreShaders", m_ProjectSettings.m_EngineAssetPath + std::string("Shaders"));

				m_SceneManager->EnqueueScene(new Scene("Empty Scene"));
				m_SceneManager->SwitchScene(0);

				EXCIMER_LOG_ERROR("Failed to load project");
			}
		}
	}
}
