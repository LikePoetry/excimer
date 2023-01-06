#include "InspectorPanel.h"

#include "Editor.h"

#include "excimer/maths/Maths.h"
#include "excimer/scene/component/ModelComponent.h"

#include "excimer/graphics/Light.h"
#include "excimer/graphics/Environment.h"

namespace MM
{
	template <>
	void ComponentEditorWidget<Excimer::Maths::Transform>(entt::registry& reg, entt::registry::entity_type e)
	{
		EXCIMER_PROFILE_FUNCTION();
		auto& transform = reg.get<Excimer::Maths::Transform>(e);

		auto rotation = glm::degrees(glm::eulerAngles(transform.GetLocalOrientation()));
		auto position = transform.GetLocalPosition();
		auto scale = transform.GetLocalScale();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		ImGui::TextUnformatted("Position");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		if (ImGui::DragFloat3("##Position", glm::value_ptr(position), 3, 0.05f))
		{
			transform.SetLocalPosition(position);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::TextUnformatted("Rotation");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation), 3, 0.05f))
		{
			float pitch = Excimer::Maths::Min(rotation.x, 89.9f);
			pitch = Excimer::Maths::Max(pitch, -89.9f);
			transform.SetLocalOrientation(glm::quat(glm::radians(glm::vec3(pitch, rotation.y, rotation.z))));
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::TextUnformatted("Scale");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 3, 0.05f))
		{
			transform.SetLocalScale(scale);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}

	template <>
	void ComponentEditorWidget<Excimer::Graphics::Light>(entt::registry& reg, entt::registry::entity_type e)
	{
		EXCIMER_PROFILE_FUNCTION();
		auto& light = reg.get<Excimer::Graphics::Light>(e);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		if (light.Type != 0)
			Excimer::ImGuiUtilities::Property("Position", light.Position);

		if (light.Type != 2)
			Excimer::ImGuiUtilities::Property("Direction", light.Direction);

		if (light.Type != 0)
			Excimer::ImGuiUtilities::Property("Radius", light.Radius, 0.0f, 100.0f);
		Excimer::ImGuiUtilities::Property("Colour", light.Colour, true, Excimer::ImGuiUtilities::PropertyFlag::ColourProperty);
		Excimer::ImGuiUtilities::Property("Intensity", light.Intensity, 0.0f, 4.0f);

		if (light.Type == 1)
			Excimer::ImGuiUtilities::Property("Angle", light.Angle, -1.0f, 1.0f);

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Light Type");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		const char* types[] = { "Directional", "Spot", "Point" };
		std::string light_current = Excimer::Graphics::Light::LightTypeToString(Excimer::Graphics::LightType(int(light.Type)));
		if (ImGui::BeginCombo("", light_current.c_str(), 0)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < 3; n++)
			{
				bool is_selected = (light_current.c_str() == types[n]);
				if (ImGui::Selectable(types[n], light_current.c_str()))
				{
					light.Type = Excimer::Graphics::Light::StringToLightType(types[n]);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();
	}

	std::string GetPrimativeName(Excimer::Graphics::PrimitiveType type)
	{
		EXCIMER_PROFILE_FUNCTION();
		switch (type)
		{
		case Excimer::Graphics::PrimitiveType::Cube:
			return "Cube";
		case Excimer::Graphics::PrimitiveType::Plane:
			return "Plane";
		case Excimer::Graphics::PrimitiveType::Quad:
			return "Quad";
		case Excimer::Graphics::PrimitiveType::Sphere:
			return "Sphere";
		case Excimer::Graphics::PrimitiveType::Pyramid:
			return "Pyramid";
		case Excimer::Graphics::PrimitiveType::Capsule:
			return "Capsule";
		case Excimer::Graphics::PrimitiveType::Cylinder:
			return "Cylinder";
		case Excimer::Graphics::PrimitiveType::Terrain:
			return "Terrain";
		case Excimer::Graphics::PrimitiveType::File:
			return "File";
		case Excimer::Graphics::PrimitiveType::None:
			return "None";
		}

		EXCIMER_LOG_ERROR("Primitive not supported");
		return "";
	};

	Excimer::Graphics::PrimitiveType GetPrimativeName(const std::string& type)
	{
		EXCIMER_PROFILE_FUNCTION();
		if (type == "Cube")
		{
			return Excimer::Graphics::PrimitiveType::Cube;
		}
		else if (type == "Quad")
		{
			return Excimer::Graphics::PrimitiveType::Quad;
		}
		else if (type == "Sphere")
		{
			return Excimer::Graphics::PrimitiveType::Sphere;
		}
		else if (type == "Pyramid")
		{
			return Excimer::Graphics::PrimitiveType::Pyramid;
		}
		else if (type == "Capsule")
		{
			return Excimer::Graphics::PrimitiveType::Capsule;
		}
		else if (type == "Cylinder")
		{
			return Excimer::Graphics::PrimitiveType::Cylinder;
		}
		else if (type == "Terrain")
		{
			return Excimer::Graphics::PrimitiveType::Terrain;
		}

		EXCIMER_LOG_ERROR("Primitive not supported");
		return Excimer::Graphics::PrimitiveType::Cube;
	};

	void TextureWidget(const char* label, Excimer::Graphics::Material* material, Excimer::Graphics::Texture2D* tex, bool flipImage, float& usingMapProperty, glm::vec4& colourProperty, const std::function<void(const std::string&)>& callback, const ImVec2& imageButtonSize = ImVec2(64, 64))
	{
		using namespace Excimer;
		if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(2);
			ImGui::Separator();

			ImGui::AlignTextToFramePadding();

			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			auto min = ImGui::GetCurrentWindow()->DC.CursorPos;
			auto max = min + imageButtonSize + ImGui::GetStyle().FramePadding;

			bool hoveringButton = ImGui::IsMouseHoveringRect(min, max, false);
			bool showTexture = !(hoveringButton && (payload != NULL && payload->IsDataType("AssetFile")));
			if (tex && showTexture)
			{
				if (ImGui::ImageButton(tex->GetHandle(), imageButtonSize, ImVec2(0.0f, flipImage ? 1.0f : 0.0f), ImVec2(1.0f, flipImage ? 0.0f : 1.0f)))
				{
					/*Excimer::Editor::GetEditor()->GetFileBrowserPanel().Open();
					Excimer::Editor::GetEditor()->GetFileBrowserPanel().SetCallback(callback);*/
				}

				if (ImGui::IsItemHovered() && tex)
				{
					ImGui::BeginTooltip();
					ImGui::Image(tex->GetHandle(), ImVec2(256, 256), ImVec2(0.0f, flipImage ? 1.0f : 0.0f), ImVec2(1.0f, flipImage ? 0.0f : 1.0f));
					ImGui::EndTooltip();
				}
			}
			else
			{
				if (ImGui::Button(tex ? "" : "Empty", imageButtonSize))
				{
					/*Excimer::Editor::GetEditor()->GetFileBrowserPanel().Open();
					Excimer::Editor::GetEditor()->GetFileBrowserPanel().SetCallback(callback);*/
				}
			}

			if (payload != NULL && payload->IsDataType("AssetFile"))
			{
				auto filePath = std::string(reinterpret_cast<const char*>(payload->Data));
				//if (Excimer::Editor::GetEditor()->IsTextureFile(filePath))
				//{
				//	if (ImGui::BeginDragDropTarget())
				//	{
				//		// Drop directly on to node and append to the end of it's children list.
				//		if (ImGui::AcceptDragDropPayload("AssetFile"))
				//		{
				//			callback(filePath);
				//			ImGui::EndDragDropTarget();

				//			ImGui::Columns(1);
				//			ImGui::Separator();
				//			ImGui::PopStyleVar();

				//			ImGui::TreePop();
				//			return;
				//		}

				//		ImGui::EndDragDropTarget();
				//	}
				//}
			}

			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			ImGui::TextUnformatted(tex ? tex->GetFilepath().c_str() : "No Texture");
			if (tex)
			{
				ImGuiUtilities::Tooltip(tex->GetFilepath());
				ImGui::Text("%u x %u", tex->GetWidth(), tex->GetHeight());
				ImGui::Text("Mip Levels : %u", tex->GetMipMapLevels());
			}
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGuiUtilities::Property("Use Map", usingMapProperty, 0.0f, 1.0f);
			ImGuiUtilities::Property("Colour", colourProperty, 0.0f, 1.0f, false, Excimer::ImGuiUtilities::PropertyFlag::ColourProperty);

			ImGui::Columns(1);

			ImGui::Separator();
			ImGui::PopStyleVar();

			ImGui::TreePop();
		}
	}

	void TextureWidget(const char* label, Excimer::Graphics::Material* material, Excimer::Graphics::Texture2D* tex, bool flipImage, float& usingMapProperty, float& amount, const std::function<void(const std::string&)>& callback, const ImVec2& imageButtonSize = ImVec2(64, 64))
	{
		using namespace Excimer;
		if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
			ImGui::Columns(2);
			ImGui::Separator();

			ImGui::AlignTextToFramePadding();

			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			auto min = ImGui::GetCurrentWindow()->DC.CursorPos;
			auto max = min + imageButtonSize + ImGui::GetStyle().FramePadding;

			bool hoveringButton = ImGui::IsMouseHoveringRect(min, max, false);
			bool showTexture = !(hoveringButton && (payload != NULL && payload->IsDataType("AssetFile")));
			if (tex && showTexture)
			{
				if (ImGui::ImageButton(tex->GetHandle(), imageButtonSize, ImVec2(0.0f, flipImage ? 1.0f : 0.0f), ImVec2(1.0f, flipImage ? 0.0f : 1.0f)))
				{
					Excimer::Editor::GetEditor()->GetFileBrowserPanel().Open();
					Excimer::Editor::GetEditor()->GetFileBrowserPanel().SetCallback(callback);
				}

				if (ImGui::IsItemHovered() && tex)
				{
					ImGui::BeginTooltip();
					ImGui::Image(tex->GetHandle(), ImVec2(256, 256), ImVec2(0.0f, flipImage ? 1.0f : 0.0f), ImVec2(1.0f, flipImage ? 0.0f : 1.0f));
					ImGui::EndTooltip();
				}
			}
			else
			{
				if (ImGui::Button(tex ? "" : "Empty", imageButtonSize))
				{
					Excimer::Editor::GetEditor()->GetFileBrowserPanel().Open();
					Excimer::Editor::GetEditor()->GetFileBrowserPanel().SetCallback(callback);
				}
			}

			if (payload != NULL && payload->IsDataType("AssetFile"))
			{
				auto filePath = std::string(reinterpret_cast<const char*>(payload->Data));
				//if (Excimer::Editor::GetEditor()->IsTextureFile(filePath))
				//{
				//	if (ImGui::BeginDragDropTarget())
				//	{
				//		// Drop directly on to node and append to the end of it's children list.
				//		if (ImGui::AcceptDragDropPayload("AssetFile"))
				//		{
				//			callback(filePath);
				//			ImGui::EndDragDropTarget();

				//			ImGui::Columns(1);
				//			ImGui::Separator();
				//			ImGui::PopStyleVar();

				//			ImGui::TreePop();
				//			return;
				//		}

				//		ImGui::EndDragDropTarget();
				//	}
				//}
			}

			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			ImGui::TextUnformatted(tex ? tex->GetFilepath().c_str() : "No Texture");
			if (tex)
			{
				ImGuiUtilities::Tooltip(tex->GetFilepath());
				ImGui::Text("%u x %u", tex->GetWidth(), tex->GetHeight());
				ImGui::Text("Mip Levels : %u", tex->GetMipMapLevels());
			}
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGuiUtilities::Property("Use Map", usingMapProperty, 0.0f, 1.0f);
			ImGuiUtilities::Property("Value", amount, 0.0f, 20.0f);

			ImGui::Columns(1);

			ImGui::Separator();
			ImGui::PopStyleVar();

			ImGui::TreePop();
		}
	}


	template <>
	void ComponentEditorWidget<Excimer::Graphics::ModelComponent>(entt::registry& reg, entt::registry::entity_type e)
	{
		EXCIMER_PROFILE_FUNCTION();
		auto& model = *reg.get<Excimer::Graphics::ModelComponent>(e).ModelRef.get();

		auto primitiveType = reg.get<Excimer::Graphics::ModelComponent>(e).ModelRef ? model.GetPrimitiveType() : Excimer::Graphics::PrimitiveType::None;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		ImGui::TextUnformatted("Primitive Type");

		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		const char* shapes[] = { "Sphere", "Cube", "Pyramid", "Capsule", "Cylinder", "Terrain", "File", "Quad", "None" };
		std::string shape_current = GetPrimativeName(primitiveType).c_str();
		if (ImGui::BeginCombo("", shape_current.c_str(), 0)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < 8; n++)
			{
				bool is_selected = (shape_current.c_str() == shapes[n]);
				if (ImGui::Selectable(shapes[n], shape_current.c_str()))
				{
					if (reg.get<Excimer::Graphics::ModelComponent>(e).ModelRef)
						model.GetMeshes().clear();

					if (strcmp(shapes[n], "File") != 0)
					{
						if (reg.get<Excimer::Graphics::ModelComponent>(e).ModelRef)
						{
							model.GetMeshes().push_back(Excimer::SharedPtr<Excimer::Graphics::Mesh>(Excimer::Graphics::CreatePrimative(GetPrimativeName(shapes[n]))));
							model.SetPrimitiveType(GetPrimativeName(shapes[n]));
						}
						else
						{
							reg.get<Excimer::Graphics::ModelComponent>(e).LoadPrimitive(GetPrimativeName(shapes[n]));
						}
					}
					else
					{
						if (reg.get<Excimer::Graphics::ModelComponent>(e).ModelRef)
							model.SetPrimitiveType(Excimer::Graphics::PrimitiveType::File);
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		if (primitiveType == Excimer::Graphics::PrimitiveType::File)
		{
			ImGui::TextUnformatted("FilePath");

			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			ImGui::TextUnformatted(model.GetFilePath().c_str());
			Excimer::ImGuiUtilities::Tooltip(model.GetFilePath());

			ImGui::PopItemWidth();
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::Separator();
		ImGui::PopStyleVar();

		int matIndex = 0;

		if (!reg.get<Excimer::Graphics::ModelComponent>(e).ModelRef)
			return;

		auto& meshes = reg.get<Excimer::Graphics::ModelComponent>(e).ModelRef->GetMeshes();
		for (auto mesh : meshes)
		{
			auto material = mesh->GetMaterial();
			std::string matName = "Material";
			matName += std::to_string(matIndex);
			matIndex++;
			if (!material)
			{
				ImGui::TextUnformatted("Empty Material");
				if (ImGui::Button("Add Material", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
					mesh->SetMaterial(Excimer::CreateSharedPtr<Excimer::Graphics::Material>());
			}
			else if (ImGui::TreeNodeEx(matName.c_str(), 0))
			{
				using namespace Excimer;
				bool flipImage = Graphics::Renderer::GetGraphicsContext()->FlipImGUITexture();

				bool twoSided = material->GetFlag(Excimer::Graphics::Material::RenderFlags::TWOSIDED);
				bool depthTested = material->GetFlag(Excimer::Graphics::Material::RenderFlags::DEPTHTEST);

				if (ImGuiUtilities::Property("Two Sided", twoSided))
					material->SetFlag(Excimer::Graphics::Material::RenderFlags::TWOSIDED, twoSided);

				if (ImGuiUtilities::Property("Depth Tested", depthTested))
					material->SetFlag(Excimer::Graphics::Material::RenderFlags::DEPTHTEST, depthTested);

				Graphics::MaterialProperties* prop = material->GetProperties();
				auto colour = glm::vec4();
				float normal = 0.0f;
				auto& textures = material->GetTextures();
				TextureWidget("Albedo", material.get(), textures.albedo.get(), flipImage, prop->albedoMapFactor, prop->albedoColour, std::bind(&Graphics::Material::SetAlbedoTexture, material, std::placeholders::_1), ImVec2(64, 64) * Application::Get().GetWindowDPI());
				ImGui::Separator();

				TextureWidget("Normal", material.get(), textures.normal.get(), flipImage, prop->normalMapFactor, normal, std::bind(&Graphics::Material::SetNormalTexture, material, std::placeholders::_1), ImVec2(64, 64) * Application::Get().GetWindowDPI());
				ImGui::Separator();

				TextureWidget("Metallic", material.get(), textures.metallic.get(), flipImage, prop->metallicMapFactor, prop->metallic, std::bind(&Graphics::Material::SetMetallicTexture, material, std::placeholders::_1), ImVec2(64, 64) * Application::Get().GetWindowDPI());
				ImGui::Separator();

				TextureWidget("Roughness", material.get(), textures.roughness.get(), flipImage, prop->roughnessMapFactor, prop->roughness, std::bind(&Graphics::Material::SetRoughnessTexture, material, std::placeholders::_1), ImVec2(64, 64) * Application::Get().GetWindowDPI());
				ImGui::Separator();

				TextureWidget("AO", material.get(), textures.ao.get(), flipImage, prop->occlusionMapFactor, normal, std::bind(&Graphics::Material::SetAOTexture, material, std::placeholders::_1), ImVec2(64, 64) * Application::Get().GetWindowDPI());
				ImGui::Separator();

				TextureWidget("Emissive", material.get(), textures.emissive.get(), flipImage, prop->emissiveMapFactor, prop->emissive, std::bind(&Graphics::Material::SetEmissiveTexture, material, std::placeholders::_1), ImVec2(64, 64) * Application::Get().GetWindowDPI());

				ImGui::Columns(2);

				ImGui::AlignTextToFramePadding();
				ImGui::TextUnformatted("WorkFlow");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);

				int workFlow = (int)material->GetProperties()->workflow;

				if (ImGui::DragInt("##WorkFlow", &workFlow, 0.3f, 0, 2))
				{
					material->GetProperties()->workflow = (float)workFlow;
				}

				ImGui::PopItemWidth();
				ImGui::NextColumn();

				material->SetMaterialProperites(*prop);
				ImGui::Columns(1);
				ImGui::TreePop();
			}
		}
	}

	template <>
	void ComponentEditorWidget<Excimer::Graphics::Environment>(entt::registry& reg, entt::registry::entity_type e)
	{
		EXCIMER_PROFILE_FUNCTION();
		auto& environment = reg.get<Excimer::Graphics::Environment>(e);
		// Disable image until texturecube is supported
		// Excimer::ImGuiUtilities::Image(environment.GetEnvironmentMap(), glm::vec2(200, 200));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		ImGui::Separator();

		ImGui::TextUnformatted("File Path");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		static char filePath[INPUT_BUF_SIZE];
		strcpy(filePath, environment.GetFilePath().c_str());

		if (ImGui::InputText("##filePath", filePath, IM_ARRAYSIZE(filePath), 0))
		{
			environment.SetFilePath(filePath);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("File Type");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);

		static char fileType[INPUT_BUF_SIZE];
		strcpy(fileType, environment.GetFileType().c_str());

		if (ImGui::InputText("##fileType", fileType, IM_ARRAYSIZE(fileType), 0))
		{
			environment.SetFileType(fileType);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Width");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		int width = environment.GetWidth();

		if (ImGui::DragInt("##Width", &width))
		{
			environment.SetWidth(width);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Height");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		int height = environment.GetHeight();

		if (ImGui::DragInt("##Height", &height))
		{
			environment.SetHeight(height);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Num Mips");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		int numMips = environment.GetNumMips();
		if (ImGui::InputInt("##NumMips", &numMips))
		{
			environment.SetNumMips(numMips);
		}

		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Columns(1);
		if (ImGui::Button("Reload", ImVec2(ImGui::GetContentRegionAvail().x, 0.0)))
			environment.Load();

		ImGui::Separator();
		ImGui::PopStyleVar();
	}
}

namespace Excimer
{
	InspectorPanel::InspectorPanel()
	{
		m_Name = ICON_MDI_INFORMATION " Inspector###inspector";
		m_SimpleName = "Inspector";
	}

	static bool init = false;
	void InspectorPanel::OnNewScene(Scene* scene)
	{
		//初始化属性名称和图标
		EXCIMER_PROFILE_FUNCTION();
		if (init)
			return;

		init = true;

		auto& registry = scene->GetRegistry();
		auto& iconMap = m_Editor->GetComponentIconMap();

#define TRIVIAL_COMPONENT(ComponentType, ComponentName)                      \
    {                                                                        \
        std::string Name;                                                    \
        if(iconMap.find(typeid(ComponentType).hash_code()) != iconMap.end()) \
            Name += iconMap[typeid(ComponentType).hash_code()];              \
        else                                                                 \
            Name += iconMap[typeid(Editor).hash_code()];                     \
        Name += "\t";                                                        \
        Name += (ComponentName);                                             \
        m_EnttEditor.registerComponent<ComponentType>(Name.c_str());         \
    }
		TRIVIAL_COMPONENT(Maths::Transform, "Transform");
		TRIVIAL_COMPONENT(Graphics::ModelComponent, "ModelComponent");
		TRIVIAL_COMPONENT(Graphics::Light, "Light");
		TRIVIAL_COMPONENT(Graphics::Environment, "Environment");
	}

	void InspectorPanel::OnImGui()
	{
		// 绘制属性列表
		EXCIMER_PROFILE_FUNCTION();

		//获取选取的Entity
		auto selected = m_Editor->GetSelected();

		if (ImGui::Begin(m_Name.c_str(), &m_Active))
		{
			//No Scene
			if (!Application::Get().GetSceneManager()->GetCurrentScene())
			{
				m_Editor->SetSelected(entt::null);
				ImGui::End();
				return;
			}

			//No Entt
			auto& registry = Application::Get().GetSceneManager()->GetCurrentScene()->GetRegistry();
			if (selected == entt::null || !registry.valid(selected))
			{
				m_Editor->SetSelected(entt::null);
				ImGui::End();
				return;
			}

			//绘制各个组件
			ImGui::BeginChild("Components", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_None);
			m_EnttEditor.RenderImGui(registry, selected);
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void InspectorPanel::SetDebugMode(bool mode)
	{
		m_DebugMode = mode;
	}

}