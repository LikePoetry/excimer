#pragma once
#include "EditorPanel.h"

#include "Editor.h";

#include <imgui/imgui.h>
DISABLE_WARNING_PUSH
DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE
#include <entt/entt.hpp>
DISABLE_WARNING_POP

namespace Excimer
{
	class SceneViewPanel :public EditorPanel
	{
	public:
		SceneViewPanel();
		~SceneViewPanel() = default;


		void OnImGui() override;
		void OnNewScene(Scene* scene) override;

	private:
		Scene* m_CurrentScene = nullptr;
		uint32_t m_Width, m_Height;
	};
}