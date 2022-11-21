#pragma once
#include "EditorPanel.h"

#include <excimer/core/Application.h>
#include <excimer/imgui/ImGuiUtilities.h>
#include <imgui/imgui.h>
namespace Excimer
{
	class Editor : public Application
	{
		friend class Application;

	public:
		Editor();
		virtual ~Editor();

		void OnEvent(Event& e) override;
		void OnImGui() override;

		void Init() override;


		void DrawMenuBar();
        void BeginDockSpace(bool gameFullScreen);
        void EndDockSpace();

        void SaveEditorSettings();


        struct EditorSettings
        {
            float m_GridSize = 10.0f;
            uint32_t m_DebugDrawFlags = 0;
            uint32_t m_Physics2DDebugFlags = 0;
            uint32_t m_Physics3DDebugFlags = 0;

            bool m_ShowGrid = true;
            bool m_ShowGizmos = true;
            bool m_ShowViewSelected = false;
            bool m_SnapQuizmo = false;
            bool m_ShowImGuiDemo = true;
            bool m_View2D = false;
            bool m_FullScreenOnPlay = false;
            float m_SnapAmount = 1.0f;
            bool m_SleepOutofFocus = true;
            float m_ImGuizmoScale = 0.25f;

            bool m_FullScreenSceneView = false;
            ImGuiUtilities::Theme m_Theme = ImGuiUtilities::Theme::Black;
            bool m_FreeAspect = true;
            float m_FixedAspect = 1.0f;
            bool m_HalfRes = false;
            float m_AspectRatio = 1.0f;

            // Camera Settings
        };

        EditorSettings& GetSettings() { return m_Settings; }


	protected:

		EditorSettings m_Settings;
        std::vector<SharedPtr<EditorPanel>> m_Panels;
	};
}