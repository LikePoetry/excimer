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
#include "excimer/graphics/renderers/DebugRenderer.h"

#include "excimer/core/Profiler.h"
#include "excimer/core/JobSystem.h"

#include "excimer/embedded/splash.inl"

#include <filesystem>

#include <cereal/archives/json.hpp>
#include <imgui/imgui.h>

namespace Excimer {

	Application* Application::s_Instance = nullptr;

	Application::Application()
		:m_Frames(0)
		, m_Updates(0)
		, m_SceneViewWidth(800)
		, m_SceneViewHeight(600)
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

		//是否全屏显示，改变尺寸需要更新交换链;
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

		uint32_t screenWidth = m_Window->GetWidth();
		uint32_t screenHeight = m_Window->GetHeight();
		m_SystemManager = CreateUniquePtr<SystemManager>();

		System::JobSystem::Context context;

		System::JobSystem::Execute(context, [](JobDispatchArgs args) {Excimer::Input::Get(); });

		//暂时不接入  声音和物理系统
		//System::JobSystem::Execute(context, [this](JobDispatchArgs args)
		//	{
		//		auto audioManager = AudioManager::Create();
		//		if (audioManager)
		//		{
		//			audioManager->OnInit();
		//			audioManager->SetPaused(true);
		//			m_SystemManager->RegisterSystem<AudioManager>(audioManager);
		//		} });

		//System::JobSystem::Execute(context, [this](JobDispatchArgs args)
		//	{
		//		m_SystemManager->RegisterSystem<SlightPhysicsEngine>();
		//		m_SystemManager->RegisterSystem<B2PhysicsEngine>(); });

		System::JobSystem::Execute(context, [this](JobDispatchArgs args)
			{ m_SceneManager->LoadCurrentList(); });

		m_ImGuiManager = CreateUniquePtr<ImGuiManager>(false);
		m_ImGuiManager->OnInit();

		m_RenderGraph = CreateUniquePtr<Graphics::RenderGraph>(screenWidth, screenHeight);
		m_CurrentState = AppState::Running;

		System::JobSystem::Wait(context);

		Graphics::Material::InitDefaultTexture();
		Graphics::Font::InitDefaultFont();
		m_RenderGraph->EnableDebugRenderer(true);
	}

	void Application::OnEvent(Event& e)
	{
		EXCIMER_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		if (m_ImGuiManager)
			m_ImGuiManager->OnEvent(e);
		if (e.Handled())
			return;

		if (m_RenderGraph)
			m_RenderGraph->OnEvent(e);

		if (e.Handled())
			return;

		if (m_SceneManager->GetCurrentScene())
			m_SceneManager->GetCurrentScene()->OnEvent(e);

		Input::Get().OnEvent(e);

	}

	void Application::OnNewScene(Scene* scene)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_RenderGraph->OnNewScene(scene);
	}

	void Application::OnRender()
	{
		EXCIMER_PROFILE_FUNCTION();
		if (!m_SceneManager->GetCurrentScene())
			return;

		if (!m_DisableMainRenderGraph)
		{
			m_RenderGraph->BeginScene(m_SceneManager->GetCurrentScene());
			m_RenderGraph->OnRender();

			// Clears debug line and point lists
			DebugRenderer::Reset();
			OnDebugDraw();
		}
	}

	void Application::OnDebugDraw()
	{
		m_SystemManager->OnDebugDraw();
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
		EXCIMER_PROFILE_FUNCTION();
		if (m_SceneManager->GetSwitchingScene())
		{
			EXCIMER_PROFILE_SCOPE("Application::SceneSwitch");
			Graphics::Renderer::GetGraphicsContext()->WaitIdle();
			m_SceneManager->ApplySceneSwitch();
			return m_CurrentState != AppState::Closing;
		}

		double now = m_Timer->GetElapsedSD();
		auto& stats = Engine::Get().Statistics();
		auto& ts = Engine::GetTimeStep();

		if (ts.GetSeconds() > 5)
		{
			EXCIMER_LOG_WARN("Large frame time {0}", ts.GetSeconds());
		}

		{

			EXCIMER_PROFILE_SCOPE("Application::TimeStepUpdates");
			ts.OnUpdate();

			ImGuiIO& io = ImGui::GetIO();
			io.DeltaTime = (float)ts.GetSeconds();

			stats.FrameTime = ts.GetMillis();
		}

		Input::Get().ResetPressed();

		m_Window->ProcessInput();


		{
			EXCIMER_PROFILE_SCOPE("Application::ImGui::NewFrame");
			ImGui::NewFrame();
		}

		{
			EXCIMER_PROFILE_SCOPE("Application::Update");
			OnUpdate(ts);
			m_Updates++;
		}

		if (m_CurrentState == AppState::Closing)
			return false;

		std::thread updateThread = std::thread(Application::UpdateSystems);

		if (!m_Minimized)
		{
			EXCIMER_PROFILE_SCOPE("Application::Render");

			Graphics::Renderer::GetRenderer()->Begin();

			OnRender();

			m_ImGuiManager->OnRender(m_SceneManager->GetCurrentScene());

			Graphics::Renderer::GetRenderer()->Present();

			Graphics::Pipeline::DeleteUnusedCache();
			Graphics::Framebuffer::DeleteUnusedCache();
			Graphics::RenderPass::DeleteUnusedCache();

			// m_ShaderLibrary->Update(ts.GetElapsedSeconds());
			m_ModelLibrary->Update((float)ts.GetElapsedSeconds());
			m_FontLibrary->Update((float)ts.GetElapsedSeconds());
			m_Frames++;
		}
		else
		{
			ImGui::Render();
		}

		{
			EXCIMER_PROFILE_SCOPE("Application::UpdateGraphicsStats");
			stats.UsedGPUMemory = Graphics::Renderer::GetGraphicsContext()->GetGPUMemoryUsed();
			stats.TotalGPUMemory = Graphics::Renderer::GetGraphicsContext()->GetTotalGPUMemory();
		}

		{
			EXCIMER_PROFILE_SCOPE("Application::WindowUpdate");
			m_Window->UpdateCursorImGui();
			m_Window->OnUpdate();
		}

		{
			EXCIMER_PROFILE_SCOPE("Wait System update");
			updateThread.join();

			//TODO 物理系统的更新
			//m_SystemManager->GetSystem<SlightPhysicsEngine>()->SyncTransforms(m_SceneManager->GetCurrentScene());
			//m_SystemManager->GetSystem<B2PhysicsEngine>()->SyncTransforms(m_SceneManager->GetCurrentScene());
		}

		if (now - m_SecondTimer > 1.0f)
		{
			EXCIMER_PROFILE_SCOPE("Application::FrameRateCalc");
			m_SecondTimer += 1.0f;

			stats.FramesPerSecond = m_Frames;
			stats.UpdatesPerSecond = m_Updates;

			m_Frames = 0;
			m_Updates = 0;
		}

		EXCIMER_PROFILE_FRAMEMARKER();

		return m_CurrentState != AppState::Closing;
	}

	void Application::UpdateSystems()
	{
		EXCIMER_PROFILE_FUNCTION();
		if (Application::Get().GetEditorState() != EditorState::Paused
			&& Application::Get().GetEditorState() != EditorState::Preview)
		{
			auto scene = Application::Get().GetSceneManager()->GetCurrentScene();

			if (!scene)
				return;

			Application::Get().GetSystemManager()->OnUpdate(Engine::GetTimeStep(), scene);
		}
	}

	void Application::OnUpdate(const TimeStep& dt)
	{
		EXCIMER_PROFILE_FUNCTION();
		if (!m_SceneManager->GetCurrentScene())
			return;

		if (Application::Get().GetEditorState() != EditorState::Paused
			&& Application::Get().GetEditorState() != EditorState::Preview)
		{
			//TODO Lua support
			/*LuaManager::Get().OnUpdate(m_SceneManager->GetCurrentScene());*/
			m_SceneManager->GetCurrentScene()->OnUpdate(dt);
		}
		m_ImGuiManager->OnUpdate(dt, m_SceneManager->GetCurrentScene());
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
		EXCIMER_PROFILE_FUNCTION();
		Graphics::Renderer::GetGraphicsContext()->WaitIdle();

		int width = e.GetWidth(), height = e.GetHeight();

		if (width == 0 || height == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		//重新生成交换链
		Graphics::Renderer::GetRenderer()->OnResize(width, height);

		if (m_RenderGraph)
			m_RenderGraph->OnResize(width, height);

		Graphics::Renderer::GetGraphicsContext()->WaitIdle();

		return false;
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

			if (!FileSystem::FileExists(filePath))
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
