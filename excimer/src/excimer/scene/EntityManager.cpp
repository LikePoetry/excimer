#include "hzpch.h"
#include "Entity.h"
#include "EntityManager.h"

#include "excimer/maths/Random.h"

namespace Excimer
{
	Entity EntityManager::Create()
	{
		EXCIMER_PROFILE_FUNCTION();
		auto e = m_Registry.create();
		m_Registry.emplace<IDComponent>(e);
		return Entity(e, m_Scene);
	}

	Entity EntityManager::Create(const std::string& name)
	{
		EXCIMER_PROFILE_FUNCTION();
		auto e = m_Registry.create();
		m_Registry.emplace<NameComponent>(e, name);
		m_Registry.emplace<IDComponent>(e);
		return Entity(e, m_Scene);
	}

	void EntityManager::Clear()
	{
		EXCIMER_PROFILE_FUNCTION();
		m_Registry.each([&](auto entity)
			{ m_Registry.destroy(entity); });

		m_Registry.clear();
	}

	Entity EntityManager::GetEntityByUUID(uint64_t id)
	{
		EXCIMER_PROFILE_FUNCTION();

		auto view = m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			auto& idComponent = m_Registry.get<IDComponent>(entity);
			if (idComponent.ID == id)
				return Entity(entity, m_Scene);
		}

		EXCIMER_LOG_WARN("Entity not found by ID");
		return Entity{};
	}
}