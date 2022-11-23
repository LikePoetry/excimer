#include "InspectorPanel.h"

#include "Editor.h"

#include "excimer/maths/Maths.h"
#include "excimer/scene/component/ModelComponent.h"
#include "excimer/graphics/Light.h"

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