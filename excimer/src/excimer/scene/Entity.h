#pragma once
#include "excimer/maths/Transform.h"
#include "excimer/scene/Scene.h"
#include "excimer/scene/SceneGraph.h"
#include "excimer/core/Profiler.h"
#include "excimer/core/StringUtilities.h"
#include "excimer/core/UUID.h"
#include "excimer/core/Profiler.h"

DISABLE_WARNING_PUSH
DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE
#include <entt/entt.hpp>
DISABLE_WARNING_POP

namespace Excimer
{
	struct IDComponent
	{
		UUID ID;

		template <typename Archive>
		void save(Archive& archive) const
		{
			uint64_t uuid = (uint64_t)ID;
			archive(uuid);
		}

		template <typename Archive>
		void load(Archive& archive)
		{
			uint64_t uuid;
			archive(uuid);

			ID = UUID(uuid);
		}
	};

	class Entity
	{
	public:
		Entity() = default;

		Entity(entt::entity handle, Scene* scene)
			: m_EntityHandle(handle)
			, m_Scene(scene)
		{
		}

		~Entity()
		{
		}

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			EXCIMER_PROFILE_FUNCTION();
#ifdef EXCIMER_DEBUG
			if (HasComponent<T>())
				EXCIMER_LOG_WARN("Attempting to add Component twice");
#endif
			return m_Scene->GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		T& GetOrAddComponent(Args&&... args)
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().get_or_emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T, typename... Args>
		void AddOrReplaceComponent(Args&&... args)
		{
			EXCIMER_PROFILE_FUNCTION();
			m_Scene->GetRegistry().emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		T& GetComponent()
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().get<T>(m_EntityHandle);
		}

		template <typename T>
		T* TryGetComponent()
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().try_get<T>(m_EntityHandle);
		}

		template <typename T>
		bool HasComponent()
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().has<T>(m_EntityHandle);
		}

		template <typename T>
		void RemoveComponent()
		{
			EXCIMER_PROFILE_FUNCTION();
			m_Scene->GetRegistry().remove<T>(m_EntityHandle);
		}

		template <typename T>
		void TryRemoveComponent()
		{
			SLIGHT_PROFILE_FUNCTION();
			if (HasComponent<T>())
				RemoveComponent<T>();
		}

		bool Active()
		{
			EXCIMER_PROFILE_FUNCTION();
			bool active = true;
			if (HasComponent<ActiveComponent>())
				active = m_Scene->GetRegistry().get<ActiveComponent>(m_EntityHandle).active;

			auto parent = GetParent();
			if (parent)
				active &= parent.Active();
			return active;
		}

		void SetActive(bool isActive)
		{
			EXCIMER_PROFILE_FUNCTION();
			GetOrAddComponent<ActiveComponent>().active = isActive;
		}

		Maths::Transform& GetTransform()
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().get<Maths::Transform>(m_EntityHandle);
		}

		const Maths::Transform& GetTransform() const
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().get<Maths::Transform>(m_EntityHandle);
		}

		uint64_t GetID()
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().get<IDComponent>(m_EntityHandle).ID;
		}

		const std::string& GetName()
		{
			EXCIMER_PROFILE_FUNCTION();
			auto nameComponent = TryGetComponent<NameComponent>();

			if (nameComponent)
				return nameComponent->name;
			else
			{
				static std::string tempName = "Entity";
				return tempName;
			}
		}

		void SetParent(Entity entity)
		{
			EXCIMER_PROFILE_FUNCTION();
			bool acceptable = false;
			auto hierarchyComponent = TryGetComponent<Hierarchy>();
			if (hierarchyComponent != nullptr)
			{
				acceptable = entity.m_EntityHandle != m_EntityHandle && (!entity.IsParent(*this)) && (hierarchyComponent->Parent() != m_EntityHandle);
			}
			else
				acceptable = entity.m_EntityHandle != m_EntityHandle;

			if (!acceptable)
			{
				EXCIMER_LOG_WARN("Failed to parent entity!");
				return;
			}

			if (hierarchyComponent)
				Hierarchy::Reparent(m_EntityHandle, entity.m_EntityHandle, m_Scene->GetRegistry(), *hierarchyComponent);
			else
			{
				m_Scene->GetRegistry().emplace<Hierarchy>(m_EntityHandle, entity.m_EntityHandle);
			}
		}

		Entity GetParent()
		{
			EXCIMER_PROFILE_FUNCTION();
			auto hierarchyComp = TryGetComponent<Hierarchy>();
			if (hierarchyComp)
				return Entity(hierarchyComp->Parent(), m_Scene);
			else
				return Entity(entt::null, nullptr);
		}

		std::vector<Entity> GetChildren()
		{
			EXCIMER_PROFILE_FUNCTION();
			std::vector<Entity> children;
			auto hierarchyComponent = TryGetComponent<Hierarchy>();
			if (hierarchyComponent)
			{
				entt::entity child = hierarchyComponent->First();
				while (child != entt::null && m_Scene->GetRegistry().valid(child))
				{
					children.emplace_back(child, m_Scene);
					hierarchyComponent = m_Scene->GetRegistry().try_get<Hierarchy>(child);
					if (hierarchyComponent)
						child = hierarchyComponent->Next();
				}
			}

			return children;
		}

		void ClearChildren()
		{
			EXCIMER_PROFILE_FUNCTION();
			auto hierarchyComponent = TryGetComponent<Hierarchy>();
			if (hierarchyComponent)
			{
				hierarchyComponent->m_First = entt::null;
			}
		}

		bool IsParent(Entity potentialParent)
		{
			EXCIMER_PROFILE_FUNCTION();
			auto nodeHierarchyComponent = m_Scene->GetRegistry().try_get<Hierarchy>(m_EntityHandle);
			if (nodeHierarchyComponent)
			{
				auto parent = nodeHierarchyComponent->Parent();
				while (parent != entt::null)
				{
					if (parent == potentialParent.m_EntityHandle)
					{
						return true;
					}
					else
					{
						nodeHierarchyComponent = m_Scene->GetRegistry().try_get<Hierarchy>(parent);
						parent = nodeHierarchyComponent ? nodeHierarchyComponent->Parent() : entt::null;
					}
				}
			}

			return false;
		}

		operator entt::entity() const
		{
			return m_EntityHandle;
		}

		operator uint32_t() const
		{
			return (uint32_t)m_EntityHandle;
		}

		operator bool() const
		{
			return m_EntityHandle != entt::null && m_Scene;
		}

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		entt::entity GetHandle() const
		{
			return m_EntityHandle;
		}

		void Destroy()
		{
			EXCIMER_PROFILE_FUNCTION();
			m_Scene->GetRegistry().destroy(m_EntityHandle);
		}

		bool Valid()
		{
			EXCIMER_PROFILE_FUNCTION();
			return m_Scene->GetRegistry().valid(m_EntityHandle) && m_Scene;
		}

	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;

		friend class EntityManager;
	};
}
