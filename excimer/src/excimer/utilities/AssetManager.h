#pragma once
#include "excimer/core/VFS.h"
#include "excimer/core/Engine.h"
#include "excimer/core/Core.h"

#include "excimer/graphics/rhi/Shader.h"
#include "excimer/utilities/TSingleton.h"

#include "excimer/graphics/Font.h"
#include "excimer/graphics/Model.h"

namespace Excimer
{
	template <typename T>
	class ResourceManager
	{
    public:
        typedef T Type;
        typedef std::string IDType;
        typedef SharedPtr<T> ResourceHandle;

        struct Resource
        {
            float timeSinceReload;
            float lastAccessed;
            ResourceHandle data;
            bool onDisk;
        };

        typedef std::unordered_map<IDType, Resource> MapType;

        typedef std::function<bool(const IDType&, ResourceHandle&)> LoadFunc;
        typedef std::function<void(ResourceHandle&)> ReleaseFunc;
        typedef std::function<bool(const IDType&, ResourceHandle&)> ReloadFunc;
        typedef std::function<IDType(const ResourceHandle&)> GetIdFunc;

        ResourceHandle GetResource(const IDType& name)
        {
            typename MapType::iterator itr = m_NameResourceMap.find(name);
            if (itr != m_NameResourceMap.end())
            {
                itr->second.lastAccessed = (float)Engine::GetTimeStep().GetElapsedSeconds();
                return itr->second.data;
            }

            ResourceHandle resourceData;
            if (!m_LoadFunc(name, resourceData))
            {
                EXCIMER_LOG_ERROR("Resource Manager could not load resource name {0} of type {1}", name, typeid(T).name());
                return ResourceHandle(nullptr);
            }

            Resource newResource;
            newResource.data = resourceData;
            newResource.timeSinceReload = 0;
            newResource.onDisk = true;
            newResource.lastAccessed = (float)Engine::GetTimeStep().GetElapsedSeconds();

            m_NameResourceMap.emplace(name, newResource);

            return resourceData;
        }

        ResourceHandle GetResource(const ResourceHandle& data)
        {
            IDType newId = m_GetIdFunc(data);

            typename MapType::iterator itr = m_NameResourceMap.find(newId);
            if (itr == m_NameResourceMap.end())
            {
                ResourceHandle resourceData = data;

                Resource newResource;
                newResource.data = resourceData;
                newResource.timeSinceReload = 0;
                newResource.onDisk = false;
                m_NameResourceMap.emplace(newId, newResource);

                return resourceData;
            }

            return itr->second.data;
        }

        void AddResource(const IDType& name, ResourceHandle data)
        {
            typename MapType::iterator itr = m_NameResourceMap.find(name);
            if (itr != m_NameResourceMap.end())
            {
                itr->second.lastAccessed = (float)Engine::GetTimeStep().GetElapsedSeconds();
                itr->second.data = data;
            }

            Resource newResource;
            newResource.data = data;
            newResource.timeSinceReload = 0;
            newResource.onDisk = true;
            newResource.lastAccessed = (float)Engine::GetTimeStep().GetElapsedSeconds();

            m_NameResourceMap.emplace(name, newResource);
        }

        void Destroy()
        {
            typename MapType::iterator itr = m_NameResourceMap.begin();
            while (itr != m_NameResourceMap.end())
            {
                m_ReleaseFunc((itr->second.data));
                ++itr;
            }
        }

        void Update(const float elapsedSeconds)
        {
            typename MapType::iterator itr = m_NameResourceMap.begin();

            static IDType keysToDelete[256];
            std::size_t keysToDeleteCount = 0;

            for (auto&& [key, value] : m_NameResourceMap)
            {
                if (value.data.GetCounter()->GetReferenceCount() == 1 && m_ExpirationTime < (elapsedSeconds - itr->second.lastAccessed))
                {
                    keysToDelete[keysToDeleteCount] = key;
                    keysToDeleteCount++;
                }
            }

            for (std::size_t i = 0; i < keysToDeleteCount; i++)
            {
                m_NameResourceMap.erase(keysToDelete[i]);
            }
        }

        bool ReloadResources()
        {
            typename MapType::iterator itr = m_NameResourceMap.begin();
            while (itr != m_NameResourceMap.end())
            {
                itr->second.timeSinceReload = 0;
                if (!m_ReloadFunc(itr->first, (itr->second.data)))
                {
                    EXCIMER_LOG_ERROR("Resource Manager could not reload resource name {0} of type {1}", itr->first, typeid(T).name());
                }
                ++itr;
            }
            return true;
        }

        bool ResourceExists(const IDType& name)
        {
            typename MapType::iterator itr = m_NameResourceMap.find(name);
            return itr != m_NameResourceMap.end();
        }

        ResourceHandle operator[](const IDType& name)
        {
            return GetResource(name);
        }

        LoadFunc& LoadFunction() { return m_LoadFunc; }
        ReleaseFunc& ReleaseFunction() { return m_ReleaseFunc; }
        ReloadFunc& ReloadFunction() { return m_ReloadFunc; }

    protected:
        MapType m_NameResourceMap = {};
        LoadFunc m_LoadFunc;
        ReleaseFunc m_ReleaseFunc;
        ReloadFunc m_ReloadFunc;
        GetIdFunc m_GetIdFunc;
        float m_ExpirationTime = 3.0f;
	};

    class ShaderLibrary : public ResourceManager<Graphics::Shader>
    {
    public:
        ShaderLibrary()
        {
            m_LoadFunc = Load;
        }

        ~ShaderLibrary()
        {
        }

        static bool Load(const std::string& filePath, SharedPtr<Graphics::Shader>& shader)
        {
            shader = SharedPtr<Graphics::Shader>(Graphics::Shader::CreateFromFile(filePath));
            return true;
        }
    };

    class ModelLibrary : public ResourceManager<Graphics::Model>
    {
    public:
        ModelLibrary()
        {
            m_LoadFunc = Load;
        }

        ~ModelLibrary()
        {
        }

        static bool Load(const std::string& filePath, SharedPtr<Graphics::Model>& model)
        {
            model = CreateSharedPtr<Graphics::Model>(filePath);
            return true;
        }
    };

    class FontLibrary : public ResourceManager<Graphics::Font>
    {
    public:
        FontLibrary()
        {
            m_LoadFunc = Load;
        }

        ~FontLibrary()
        {
        }

        static bool Load(const std::string& filePath, SharedPtr<Graphics::Font>& font)
        {
            font = CreateSharedPtr<Graphics::Font>(filePath);
            return true;
        }
    };
}