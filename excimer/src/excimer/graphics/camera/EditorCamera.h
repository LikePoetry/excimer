#pragma once
#include "excimer/graphics/camera/CameraController.h"

namespace Excimer
{
	class EXCIMER_EXPORT EditorCameraController :public CameraController
	{
	public:
		EditorCameraController();
		~EditorCameraController();

		virtual void HandleMouse(Maths::Transform& transform, float dt, float xpos, float ypos) override;
		virtual void HandleKeyboard(Maths::Transform& transform, float dt) override;

		void UpdateScroll(Maths::Transform& transform, float offset, float dt) override;

		void SetMode(bool is2D)
		{
			m_2DMode = is2D;
		}

		bool GetMode()
		{
			return m_2DMode;
		}

		void SetCamera(Camera& camera)
		{
			m_CurrentCamera = &camera;
		}

		void StopMovement();

	private:
		Camera* m_CurrentCamera = nullptr;

		bool m_2DMode = false;
		glm::vec2 m_StoredCursorPos;
	};
}