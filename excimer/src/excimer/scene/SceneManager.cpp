#include "hzpch.h"
#include "SceneManager.h"

#include "excimer/core/Application.h"
#include "excimer/core/VFS.h"
#include "excimer/core/StringUtilities.h"
#include "Scene.h"

namespace Excimer
{
    SceneManager::SceneManager()
        : m_SceneIdx(0)
        , m_CurrentScene(nullptr)
    {
    }

    SceneManager::~SceneManager()
    {
        m_SceneIdx = 0;

        if (m_CurrentScene)
        {
            EXCIMER_LOG_INFO("[SceneManager] - Exiting scene : {0}", m_CurrentScene->GetSceneName());
            m_CurrentScene->OnCleanupScene();
        }

        m_vpAllScenes.clear();
    }

    void SceneManager::SwitchScene()
    {
        SwitchScene((m_SceneIdx + 1) % m_vpAllScenes.size());
    }

    void SceneManager::SwitchScene(int idx)
    {
        m_QueuedSceneIndex = idx;
        m_SwitchingScenes = true;
    }

    void SceneManager::SwitchScene(const std::string& name)
    {
        bool found = false;
        m_SwitchingScenes = true;
        uint32_t idx = 0;
        for (uint32_t i = 0; !found && i < m_vpAllScenes.size(); ++i)
        {
            if (m_vpAllScenes[i]->GetSceneName() == name)
            {
                found = true;
                idx = i;
                break;
            }
        }

        if (found)
        {
            SwitchScene(idx);
        }
        else
        {
            EXCIMER_LOG_ERROR("[SceneManager] - Unknown Scene Alias : {0}", name.c_str());
        }
    }

    void SceneManager::ApplySceneSwitch()
    {
        if (m_SwitchingScenes == false)
        {
            if (m_CurrentScene)
                return;

            if (m_vpAllScenes.empty())
                m_vpAllScenes.push_back(CreateSharedPtr<Scene>("NewScene"));

            m_QueuedSceneIndex = 0;
        }

        if (m_QueuedSceneIndex < 0 || m_QueuedSceneIndex >= static_cast<int>(m_vpAllScenes.size()))
        {
           EXCIMER_LOG_ERROR("[SceneManager] - Invalid Scene Index : {0}", m_QueuedSceneIndex);
            m_QueuedSceneIndex = 0;
        }

        auto& app = Application::Get();

        // Clear up old scene
        if (m_CurrentScene)
        {
           EXCIMER_LOG_INFO("[SceneManager] - Exiting scene : {0}", m_CurrentScene->GetSceneName());
            //app.GetSystem<ExcimerPhysicsEngine>()->SetPaused(true);

            m_CurrentScene->OnCleanupScene();
            app.OnExitScene();
        }

        m_SceneIdx = m_QueuedSceneIndex;
        m_CurrentScene = m_vpAllScenes[m_QueuedSceneIndex].get();

        // Initialise new scene
        //app.GetSystem<ExcimerPhysicsEngine>()->SetDefaults();
        //app.GetSystem<B2PhysicsEngine>()->SetDefaults();
        //app.GetSystem<ExcimerPhysicsEngine>()->SetPaused(false);

        std::string physicalPath;
        if (Excimer::VFS::Get().ResolvePhysicalPath("//Scenes/" + m_CurrentScene->GetSceneName() + ".lsn", physicalPath))
        {
            auto newPath = StringUtilities::RemoveName(physicalPath);
            m_CurrentScene->Deserialise(newPath, false);
        }

        auto screenSize = app.GetWindowSize();
        m_CurrentScene->SetScreenSize(static_cast<uint32_t>(screenSize.x), static_cast<uint32_t>(screenSize.y));

        if (app.GetEditorState() == EditorState::Play)
            m_CurrentScene->OnInit();

        Application::Get().OnNewScene(m_CurrentScene);

       EXCIMER_LOG_INFO("[SceneManager] - Scene switched to : {0}", m_CurrentScene->GetSceneName().c_str());

        m_SwitchingScenes = false;
    }

    std::vector<std::string> SceneManager::GetSceneNames()
    {
        std::vector<std::string> names;

        for (auto& scene : m_vpAllScenes)
        {
            names.push_back(scene->GetSceneName());
        }

        return names;
    }

    void SceneManager::EnqueueSceneFromFile(const std::string& filePath)
    {
        m_SceneFilePaths.push_back(filePath);

        auto name = StringUtilities::RemoveFilePathExtension(StringUtilities::GetFileName(filePath));
        auto scene = new Scene(name);
        EnqueueScene(scene);
    }

    void SceneManager::EnqueueScene(Scene* scene)
    {
        m_vpAllScenes.push_back(SharedPtr<Scene>(scene));
       EXCIMER_LOG_INFO("[SceneManager] - Enqueued scene : {0}", scene->GetSceneName().c_str());
    }

    void SceneManager::LoadCurrentList()
    {
        for (auto& filePath : m_SceneFilePathsToLoad)
        {
            std::string newPath;
            VFS::Get().AbsoulePathToVFS(filePath, newPath);
            EnqueueSceneFromFile(filePath);
        }

        m_SceneFilePathsToLoad.clear();
    }

    const std::vector<std::string>& SceneManager::GetSceneFilePaths()
    {
        m_SceneFilePaths.clear();
        for (auto scene : m_vpAllScenes)
            m_SceneFilePaths.push_back("//Scenes/" + scene->GetSceneName());
        return m_SceneFilePaths;
    }
}