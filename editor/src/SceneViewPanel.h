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
		void ToolBar();
		void OnNewScene(Scene* scene) override;

		void Resize(uint32_t width, uint32_t height);
	private:
		SharedPtr<Graphics::Texture2D> m_GameViewTexture = nullptr;
		Scene* m_CurrentScene = nullptr;
		uint32_t m_Width, m_Height;
	};
}