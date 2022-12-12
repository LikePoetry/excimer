#pragma once
#include "EditorPanel.h"
#include "excimer/graphics/camera/EditorCamera.h"

#include <excimer/core/Application.h>
#include <excimer/imgui/ImGuiUtilities.h>
#include <imgui/imgui.h>
namespace Excimer
{
    class Scene;
    class Event;
    class WindowCloseEvent;
    class WindowResizeEvent;
    class WindowFileEvent;
    class TimeStep;

    namespace Graphics
    {
        class Texture2D;
        class GridRenderer;
        class Mesh;
    }

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

        void SetImGuizmoOperation(uint32_t operation)
        {
            m_ImGuizmoOperation = operation;
        }
        uint32_t GetImGuizmoOperation() const
        {
            return m_ImGuizmoOperation;
        }

        void OnNewScene(Scene* scene) override;
        void OnImGuizmo();

        void OnUpdate(const TimeStep& ts) override;

        void Draw3DGrid();

        void OnRender() override;

        void SaveEditorSettings();

        //根据射线选中物体
        void SelectObject(const Maths::Ray& ray);

        glm::vec2 m_SceneViewPanelPosition;
        //获取屏幕射线
        Maths::Ray GetScreenRay(int x, int y, Camera* camera, int width, int height);

        void SetSelected(entt::entity entity)
        {
            m_SelectedEntity = entity;
        }
        entt::entity GetSelected() const
        {
            return m_SelectedEntity;
        }

        //获取组件的小图标
        std::unordered_map<size_t, const char*>& GetComponentIconMap()
        {
            return m_ComponentIconMap;
        }

        static Editor* GetEditor() { return s_Editor; }

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
            ImGuiUtilities::Theme m_Theme = ImGuiUtilities::Theme::Dark;
            bool m_FreeAspect = true;
            float m_FixedAspect = 1.0f;
            bool m_HalfRes = false;
            float m_AspectRatio = 1.0f;

            // Camera Settings
        };

        EditorSettings& GetSettings() { return m_Settings; }

        // 是否在视图区域，视图相机是否接受鼠标事件
        void SetSceneViewActive(bool active) { m_SceneViewActive = active; }

        Camera* GetCamera() const
        {
            return m_EditorCamera.get();
        }

        void CreateGridRenderer();
        const SharedPtr<Graphics::GridRenderer>& GetGridRenderer();

        EditorCameraController& GetEditorCameraController()
        {
            return m_EditorCameraController;
        }

        Maths::Transform& GetEditorCameraTransform()
        {
            return m_EditorCameraTransform;
        }

	protected:

        uint32_t m_ImGuizmoOperation = 14463;
        entt::entity m_SelectedEntity;

		EditorSettings m_Settings;
        std::vector<SharedPtr<EditorPanel>> m_Panels;

        std::unordered_map<size_t, const char*> m_ComponentIconMap;

        Camera* m_CurrentCamera = nullptr;
        EditorCameraController m_EditorCameraController;
        Maths::Transform m_EditorCameraTransform;
        SharedPtr<Camera> m_EditorCamera = nullptr;
        SharedPtr<Graphics::GridRenderer> m_GridRenderer;

        bool m_SceneViewActive = false;

        static Editor* s_Editor;

        glm::vec4 colourProperty = glm::vec4(1.0f);
        int thickness = 0;
        glm::vec3 pointTips;
        glm::vec3 camera_position;

        std::vector<glm::vec3> pointVector;

	};


}