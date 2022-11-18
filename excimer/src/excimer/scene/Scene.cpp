#include "hzpch.h"
#include "Scene.h"
#include "excimer/core/OS/Input.h"
#include "excimer/core/Application.h"
#include "excimer/graphics/RHI/GraphicsContext.h"
#include "excimer/graphics/Renderers/RenderGraph.h"
#include "excimer/graphics/Camera/Camera.h"
#include "excimer/graphics/Sprite.h"
#include "excimer/graphics/AnimatedSprite.h"
#include "excimer/utilities/TimeStep.h"
//#include "Audio/AudioManager.h"
//#include "Physics/SlightPhysicsEngine/SlightPhysicsEngine.h"
//#include "Physics/SlightPhysicsEngine/CollisionShapes/SphereCollisionShape.h"
//#include "Physics/SlightPhysicsEngine/CollisionShapes/CuboidCollisionShape.h"
//#include "Physics/SlightPhysicsEngine/CollisionShapes/PyramidCollisionShape.h"

#include "excimer/events/Event.h"
#include "excimer/events/ApplicationEvent.h"

#include "excimer/maths/Transform.h"
#include "excimer/core/OS/FileSystem.h"
#include "excimer/scene/Component/Components.h"
//#include "Scripting/Lua/LuaScriptComponent.h"
//#include "Scripting/Lua/LuaManager.h"
#include "excimer/graphics/MeshFactory.h"
#include "excimer/graphics/Light.h"
#include "excimer/graphics/Model.h"
#include "excimer/graphics/Environment.h"
#include "excimer/scene/EntityManager.h"
//#include "excimer/scene/Component/SoundComponent.h"
#include "excimer/scene/Component/ModelComponent.h"
#include "SceneGraph.h"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <entt/entity/registry.hpp>
#include <sol/sol.hpp>

namespace Excimer
{
	Scene::Scene(const std::string& name)
		: m_SceneName(name)
		, m_ScreenWidth(0)
		, m_ScreenHeight(0)
	{
		m_EntityManager = CreateUniquePtr<EntityManager>(this);
		//m_EntityManager->AddDependency<RigidBody3DComponent, Maths::Transform>();
		//m_EntityManager->AddDependency<RigidBody2DComponent, Maths::Transform>();
		m_EntityManager->AddDependency<Camera, Maths::Transform>();
		m_EntityManager->AddDependency<Graphics::ModelComponent, Maths::Transform>();
		m_EntityManager->AddDependency<Graphics::Light, Maths::Transform>();
		m_EntityManager->AddDependency<Graphics::Sprite, Maths::Transform>();
		m_EntityManager->AddDependency<Graphics::AnimatedSprite, Maths::Transform>();
		m_EntityManager->AddDependency<Graphics::Font, Maths::Transform>();

		m_SceneGraph = CreateUniquePtr<SceneGraph>();
		m_SceneGraph->Init(m_EntityManager->GetRegistry());
	}

	Scene::~Scene()
	{
		m_EntityManager->Clear();
	}

	entt::registry& Scene::GetRegistry()
	{
		return m_EntityManager->GetRegistry();
	}

	void Scene::OnInit()
	{
		EXCIMER_PROFILE_FUNCTION();
		//LuaManager::Get().GetState().set("registry", &m_EntityManager->GetRegistry());
		//LuaManager::Get().GetState().set("scene", this);

		//// Physics setup
		//auto physics3DSytem = Application::Get().GetSystem<SlightPhysicsEngine>();
		//physics3DSytem->SetDampingFactor(m_Settings.Physics3DSettings.Dampening);
		//physics3DSytem->SetIntegrationType((IntegrationType)m_Settings.Physics3DSettings.IntegrationTypeIndex);
		//physics3DSytem->SetMaxUpdatesPerFrame(m_Settings.Physics3DSettings.m_MaxUpdatesPerFrame);
		//physics3DSytem->SetPositionIterations(m_Settings.Physics3DSettings.PositionIterations);
		//physics3DSytem->SetVelocityIterations(m_Settings.Physics3DSettings.VelocityIterations);
		//physics3DSytem->SetBroadphaseType(BroadphaseType(m_Settings.Physics3DSettings.BroadPhaseTypeIndex));

		//LuaManager::Get().OnInit(this);
	}

	void Scene::OnCleanupScene()
	{
		EXCIMER_PROFILE_FUNCTION();
		DeleteAllGameObjects();

		/*LuaManager::Get().GetState().collect_garbage();

		auto audioManager = Application::Get().GetSystem<AudioManager>();
		if (audioManager)
		{
			audioManager->ClearNodes();
		}*/
	};

	void Scene::DeleteAllGameObjects()
	{
		EXCIMER_PROFILE_FUNCTION();
		m_EntityManager->Clear();
	}

	void Scene::OnUpdate(const TimeStep& timeStep)
	{
		EXCIMER_PROFILE_FUNCTION();
		const glm::vec2& mousePos = Input::Get().GetMousePosition();

		auto defaultCameraControllerView = m_EntityManager->GetEntitiesWithType<DefaultCameraController>();

		if (!defaultCameraControllerView.Empty())
		{
			auto& cameraController = defaultCameraControllerView.Front().GetComponent<DefaultCameraController>();
			auto trans = defaultCameraControllerView.Front().TryGetComponent<Maths::Transform>();
			if (Application::Get().GetSceneActive() && trans && cameraController.GetController())
			{
				cameraController.GetController()->HandleMouse(*trans, (float)timeStep.GetSeconds(), mousePos.x, mousePos.y);
				cameraController.GetController()->HandleKeyboard(*trans, (float)timeStep.GetSeconds());
			}
		}

		m_SceneGraph->Update(m_EntityManager->GetRegistry());

		auto animatedSpriteView = m_EntityManager->GetEntitiesWithType<Graphics::AnimatedSprite>();

		for (auto entity : animatedSpriteView)
		{
			auto& animSprite = entity.GetComponent<Graphics::AnimatedSprite>();
			animSprite.OnUpdate((float)timeStep.GetSeconds());
		}
	}

	void Scene::OnEvent(Event& e)
	{
		EXCIMER_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Scene::OnWindowResize));
	}

	bool Scene::OnWindowResize(WindowResizeEvent& e)
	{
		EXCIMER_PROFILE_FUNCTION();
		if (!Application::Get().GetSceneActive())
			return false;

		auto cameraView = m_EntityManager->GetRegistry().view<Camera>();
		if (!cameraView.empty())
		{
			m_EntityManager->GetRegistry().get<Camera>(cameraView.front()).SetAspectRatio(static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight()));
		}

		return false;
	}

	void Scene::SetScreenSize(uint32_t width, uint32_t height)
	{
		m_ScreenWidth = width;
		m_ScreenHeight = height;

		auto cameraView = m_EntityManager->GetRegistry().view<Camera>();
		if (!cameraView.empty())
		{
			m_EntityManager->GetRegistry().get<Camera>(cameraView.front()).SetAspectRatio(static_cast<float>(m_ScreenWidth) / static_cast<float>(m_ScreenHeight));
		}
	}

	//#define ALL_COMPONENTSV1 Maths::Transform, NameComponent, ActiveComponent, Hierarchy, Camera, LuaScriptComponent, Graphics::Model, Graphics::Light, RigidBody3DComponent, Graphics::Environment, Graphics::Sprite, RigidBody2DComponent, DefaultCameraController
	//
	//#define ALL_COMPONENTSV2 ALL_COMPONENTSV1, Graphics::AnimatedSprite
	//#define ALL_COMPONENTSV3 ALL_COMPONENTSV2, SoundComponent
	//#define ALL_COMPONENTSV4 ALL_COMPONENTSV3, Listener
	//#define ALL_COMPONENTSV5 ALL_COMPONENTSV4, IDComponent
	//#define ALL_COMPONENTSV6 ALL_COMPONENTSV5, Graphics::ModelComponent
	//#define ALL_COMPONENTSV7 ALL_COMPONENTSV6, AxisConstraintComponent
	//#define ALL_COMPONENTSV8 ALL_COMPONENTSV7, TextComponent

#define ALL_COMPONENTSV1 Maths::Transform, NameComponent, ActiveComponent, Hierarchy, Camera, Graphics::Model, Graphics::Light, Graphics::Environment, Graphics::Sprite, DefaultCameraController

#define ALL_COMPONENTSV2 ALL_COMPONENTSV1, Graphics::AnimatedSprite
//#define ALL_COMPONENTSV3 ALL_COMPONENTSV2, SoundComponent
//#define ALL_COMPONENTSV4 ALL_COMPONENTSV3, Listener
#define ALL_COMPONENTSV5 ALL_COMPONENTSV2, IDComponent
#define ALL_COMPONENTSV6 ALL_COMPONENTSV5, Graphics::ModelComponent
//#define ALL_COMPONENTSV7 ALL_COMPONENTSV6, AxisConstraintComponent
#define ALL_COMPONENTSV8 ALL_COMPONENTSV6, TextComponent
	void Scene::Serialise(const std::string& filePath, bool binary)
	{
		EXCIMER_PROFILE_FUNCTION();
		EXCIMER_LOG_INFO("Scene saved - {0}", filePath);
		std::string path = filePath;
		path += m_SceneName; // StringUtilities::RemoveSpaces(m_SceneName);

		m_SceneSerialisationVersion = SceneSerialisationVersion;

		if (binary)
		{
			path += std::string(".bin");

			std::ofstream file(path, std::ios::binary);

			{
				// output finishes flushing its contents when it goes out of scope
				cereal::BinaryOutputArchive output{ file };
				output(*this);
				entt::snapshot{ m_EntityManager->GetRegistry() }.entities(output).component<ALL_COMPONENTSV8>(output);
			}
			file.close();
		}
		else
		{
			std::stringstream storage;
			path += std::string(".lsn");

			{
				// output finishes flushing its contents when it goes out of scope
				cereal::JSONOutputArchive output{ storage };
				output(*this);
				entt::snapshot{ m_EntityManager->GetRegistry() }.entities(output).component<ALL_COMPONENTSV8>(output);
			}
			FileSystem::WriteTextFile(path, storage.str());
		}
	}

	void Scene::Deserialise(const std::string& filePath, bool binary)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_EntityManager->Clear();
		m_SceneGraph->DisableOnConstruct(true, m_EntityManager->GetRegistry());
		std::string path = filePath;
		path += m_SceneName; // StringUtilities::RemoveSpaces(m_SceneName);

		if (binary)
		{
			path += std::string(".bin");

			if (!FileSystem::FileExists(path))
			{
				EXCIMER_LOG_ERROR("No saved scene file found {0}", path);
				return;
			}

			try
			{
				std::ifstream file(path, std::ios::binary);
				cereal::BinaryInputArchive input(file);
				input(*this);
				if (m_SceneSerialisationVersion < 2)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV1>(input);
				else if (m_SceneSerialisationVersion == 3)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV2>(input);
				/*else if (m_SceneSerialisationVersion == 4)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV3>(input);
				else if (m_SceneSerialisationVersion == 5)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV4>(input);*/
				else if (m_SceneSerialisationVersion == 6)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV5>(input);
				else if (m_SceneSerialisationVersion == 7)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV6>(input);
				/*else if (m_SceneSerialisationVersion >= 8 && m_SceneSerialisationVersion < 14)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV7>(input);*/
				else if (m_SceneSerialisationVersion >= 14)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV8>(input);

				if (m_SceneSerialisationVersion < 6)
				{
					m_EntityManager->GetRegistry().each([&](auto entity)
						{ m_EntityManager->GetRegistry().emplace<IDComponent>(entity, Random64::Rand(0, std::numeric_limits<uint64_t>::max())); });
				}

				if (m_SceneSerialisationVersion < 7)
				{
					m_EntityManager->GetRegistry().each([&](auto entity)
						{
							Graphics::Model* model;
							if (model = m_EntityManager->GetRegistry().try_get<Graphics::Model>(entity))
							{
								Graphics::Model* modelCopy = new Graphics::Model(*model);
								m_EntityManager->GetRegistry().emplace<Graphics::ModelComponent>(entity, SharedPtr<Graphics::Model>(modelCopy));
								m_EntityManager->GetRegistry().remove<Graphics::Model>(entity);
							} });
				}
			}
			catch (...)
			{
				EXCIMER_LOG_ERROR("Failed to load scene - {0}", path);
			}
		}
		else
		{
			path += std::string(".lsn");

			if (!FileSystem::FileExists(path))
			{
				EXCIMER_LOG_ERROR("No saved scene file found {0}", path);
				return;
			}
			try
			{
				std::string data = FileSystem::ReadTextFile(path);
				std::istringstream istr;
				istr.str(data);
				cereal::JSONInputArchive input(istr);
				input(*this);

				if (m_SceneSerialisationVersion < 2)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV1>(input);
				else if (m_SceneSerialisationVersion == 3)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV2>(input);
				/*else if (m_SceneSerialisationVersion == 4)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV3>(input);
				else if (m_SceneSerialisationVersion == 5)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV4>(input);*/
				else if (m_SceneSerialisationVersion == 6)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV5>(input);
				else if (m_SceneSerialisationVersion == 7)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV6>(input);
				/*else if (m_SceneSerialisationVersion >= 8 && m_SceneSerialisationVersion < 14)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV7>(input);*/
				else if (m_SceneSerialisationVersion >= 14)
					entt::snapshot_loader{ m_EntityManager->GetRegistry() }.entities(input).component<ALL_COMPONENTSV8>(input);

				if (m_SceneSerialisationVersion < 6)
				{
					m_EntityManager->GetRegistry().each([&](auto entity)
						{ m_EntityManager->GetRegistry().emplace<IDComponent>(entity, Random64::Rand(0, std::numeric_limits<uint64_t>::max())); });
				}

				if (m_SceneSerialisationVersion < 7)
				{
					m_EntityManager->GetRegistry().each([&](auto entity)
						{
							Graphics::Model* model;
							if (model = m_EntityManager->GetRegistry().try_get<Graphics::Model>(entity))
							{
								Graphics::Model* modelCopy = new Graphics::Model(*model);
								m_EntityManager->GetRegistry().emplace<Graphics::ModelComponent>(entity, SharedPtr<Graphics::Model>(modelCopy));
								m_EntityManager->GetRegistry().remove<Graphics::Model>(entity);
							} });
				}
			}
			catch (...)
			{
				EXCIMER_LOG_ERROR("Failed to load scene - {0}", path);
			}
		}

		m_SceneGraph->DisableOnConstruct(false, m_EntityManager->GetRegistry());
		Application::Get().OnNewScene(this);
	}

	void Scene::UpdateSceneGraph()
	{
		EXCIMER_PROFILE_FUNCTION();
		m_SceneGraph->Update(m_EntityManager->GetRegistry());
	}

	template <typename T>
	static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
	{
		if (registry.has<T>(src))
		{
			auto& srcComponent = registry.get<T>(src);
			registry.emplace_or_replace<T>(dst, srcComponent);
		}
	}

	template <typename... Component>
	static void CopyEntity(entt::entity dst, entt::entity src, entt::registry& registry)
	{
		(CopyComponentIfExists<Component>(dst, src, registry), ...);
	}

	Entity Scene::CreateEntity()
	{
		return m_EntityManager->Create();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		EXCIMER_PROFILE_FUNCTION();
		return m_EntityManager->Create(name);
	}

	Entity Scene::GetEntityByUUID(uint64_t id)
	{
		EXCIMER_PROFILE_FUNCTION();
		return m_EntityManager->GetEntityByUUID(id);
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		EXCIMER_PROFILE_FUNCTION();
		DuplicateEntity(entity, Entity(entt::null, nullptr));
	}

	void Scene::DuplicateEntity(Entity entity, Entity parent)
	{
		EXCIMER_PROFILE_FUNCTION();
		m_SceneGraph->DisableOnConstruct(true, m_EntityManager->GetRegistry());

		Entity newEntity = m_EntityManager->Create();

		CopyEntity<ALL_COMPONENTSV8>(newEntity.GetHandle(), entity.GetHandle(), m_EntityManager->GetRegistry());
		newEntity.GetComponent<IDComponent>().ID = UUID();

		auto hierarchyComponent = newEntity.TryGetComponent<Hierarchy>();
		if (hierarchyComponent)
		{
			hierarchyComponent->m_First = entt::null;
			hierarchyComponent->m_Parent = entt::null;
			hierarchyComponent->m_Next = entt::null;
			hierarchyComponent->m_Prev = entt::null;
		}

		auto children = entity.GetChildren();
		std::vector<Entity> copiedChildren;

		for (auto child : children)
		{
			DuplicateEntity(child, newEntity);
		}

		if (parent)
			newEntity.SetParent(parent);

		m_SceneGraph->DisableOnConstruct(false, m_EntityManager->GetRegistry());
	}
}
