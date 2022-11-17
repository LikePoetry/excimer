#pragma once
#include "excimer/core/Core.h"
#include "excimer/events/KeyEvent.h"
#include "excimer/events/Event.h"
#include "excimer/events/MouseEvent.h"
#include "excimer/events/ApplicationEvent.h"

#include "excimer/core/Reference.h"

namespace Excimer
{
	class Scene;
	class TimeStep;

	namespace Graphics
	{
		class IMGUIRenderer;
	}

    class EXCIMER_EXPORT ImGuiManager
    {
    public:
        ImGuiManager(bool clearScreen = false);
        ~ImGuiManager();

        void OnInit();
        void OnUpdate(const TimeStep& dt, Scene* scene);
        void OnEvent(Event& event);
        void OnRender(Scene* scene);
        void OnNewScene(Scene* scene);

    private:
        bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
        bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
        bool OnMouseMovedEvent(MouseMovedEvent& e);
        bool OnMouseScrolledEvent(MouseScrolledEvent& e);
        bool OnKeyPressedEvent(KeyPressedEvent& e);
        bool OnKeyReleasedEvent(KeyReleasedEvent& e);
        bool OnKeyTypedEvent(KeyTypedEvent& e);
        bool OnwindowResizeEvent(WindowResizeEvent& e);

        void SetImGuiKeyCodes();
        void SetImGuiStyle();
        void AddIconFont();

        float m_FontSize;
        float m_DPIScale;

        UniquePtr<Graphics::IMGUIRenderer> m_IMGUIRenderer;
        bool m_ClearScreen;
    };
}