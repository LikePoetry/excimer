#include "hzpch.h"
#include "EditorCamera.h"

#include "excimer/graphics/camera/Camera.h"
#include "excimer/core/Application.h"
#include "excimer/core/os/Input.h"
#include "excimer/core/os/Window.h"

namespace Excimer
{
	EditorCameraController::EditorCameraController()
	{
		m_FocalPoint = glm::vec3();
		m_Velocity = glm::vec3(0.0f);
		m_RotateVelocity = glm::vec2(0.0f);
		m_PreviousCurserPos = glm::vec3(0.0f);
		m_MouseSensitivity = 0.00001f;
		m_ZoomDampeningFactor = 0.00001f;
		m_DampeningFactor = 0.00001f;
		m_RotateDampeningFactor = 0.0000001f;
	}

	EditorCameraController::~EditorCameraController()
	{
	}

	void EditorCameraController::HandleMouse(Maths::Transform& transform, float dt, float xpos, float ypos)
	{
		EXCIMER_PROFILE_FUNCTION();
		if (m_2DMode)
		{
			if (Input::Get().GetMouseHeld(InputCode::MouseKey::ButtonRight))
			{
				m_MouseSensitivity = 0.0005f;
				glm::vec3 position = transform.GetLocalPosition();
				position.x -= (xpos - m_PreviousCurserPos.x) /** camera->GetScale() */ * m_MouseSensitivity * 0.5f;
				position.y += (ypos - m_PreviousCurserPos.y) /** camera->GetScale() */ * m_MouseSensitivity * 0.5f;
				transform.SetLocalPosition(position);
			}
		}
		else
		{
			static bool mouseHeld = false;
			if (Input::Get().GetMouseClicked(InputCode::MouseKey::ButtonRight))
			{
				mouseHeld = true;
				Application::Get().GetWindow()->HideMouse(true);
				Input::Get().SetMouseMode(MouseMode::Captured);
				m_StoredCursorPos = glm::vec2(xpos, ypos);
				m_PreviousCurserPos = m_StoredCursorPos;
			}

			if (Input::Get().GetMouseHeld(InputCode::MouseKey::ButtonRight))
			{
				//�����ת����
				m_MouseSensitivity = 0.0002f;
				m_RotateVelocity = glm::vec2((xpos - m_PreviousCurserPos.x), (ypos - m_PreviousCurserPos.y)) * m_MouseSensitivity * 10.0f;
			}
			else
			{
				if (mouseHeld)
				{
					mouseHeld = false;
					Application::Get().GetWindow()->HideMouse(false);
					Application::Get().GetWindow()->SetMousePosition(m_StoredCursorPos);
					Input::Get().SetMouseMode(MouseMode::Visible);
				}
			}

			if (glm::length(m_RotateVelocity) > Maths::M_EPSILON)
			{
				glm::quat rotation = transform.GetLocalOrientation();
				glm::quat rotationX = glm::angleAxis(-m_RotateVelocity.y, glm::vec3(1.0f, 0.0f, 0.0f));
				glm::quat rotationY = glm::angleAxis(-m_RotateVelocity.x, glm::vec3(0.0f, 1.0f, 0.0f));

				rotation = rotationY * rotation;
				rotation = rotation * rotationX;

				m_PreviousCurserPos = glm::vec2(xpos, ypos);
				transform.SetLocalOrientation(rotation);
			}
		}

		m_RotateVelocity = m_RotateVelocity * pow(m_RotateDampeningFactor, dt);

		UpdateScroll(transform, Input::Get().GetScrollOffset(), dt);
	}

	void EditorCameraController::HandleKeyboard(Maths::Transform& transform, float dt)
	{
		if (m_2DMode)
		{
			glm::vec3 up = glm::vec3(0, 1, 0), right = glm::vec3(1, 0, 0);

			m_CameraSpeed = /*camera->GetScale() **/ dt * 20.0f;

			if (Input::Get().GetKeyHeld(Excimer::InputCode::Key::A))
			{
				m_Velocity -= right * m_CameraSpeed;
			}

			if (Input::Get().GetKeyHeld(Excimer::InputCode::Key::D))
			{
				m_Velocity += right * m_CameraSpeed;
			}

			if (Input::Get().GetKeyHeld(Excimer::InputCode::Key::W))
			{
				m_Velocity += up * m_CameraSpeed;
			}

			if (Input::Get().GetKeyHeld(Excimer::InputCode::Key::S))
			{
				m_Velocity -= up * m_CameraSpeed;
			}

			if (glm::length(m_Velocity) > Maths::M_EPSILON)
			{
				glm::vec3 position = transform.GetLocalPosition();
				position += m_Velocity * dt;
				m_Velocity = m_Velocity * pow(m_DampeningFactor, dt);

				transform.SetLocalPosition(position);
			}
		}
		else
		{

			float multiplier = 1000.0f;

			if (Input::Get().GetKeyHeld(InputCode::Key::LeftShift))
			{
				multiplier = 10000.0f;
			}
			else if (Input::Get().GetKeyHeld(InputCode::Key::LeftAlt))
			{
				multiplier = 50.0f;
			}

			m_CameraSpeed = multiplier * dt;

			if (Input::Get().GetMouseHeld(InputCode::MouseKey::ButtonRight))
			{
				if (Input::Get().GetKeyHeld(InputCode::Key::W))
				{
					m_Velocity -= transform.GetForwardDirection() * m_CameraSpeed;
				}

				if (Input::Get().GetKeyHeld(InputCode::Key::S))
				{
					m_Velocity += transform.GetForwardDirection() * m_CameraSpeed;
				}

				if (Input::Get().GetKeyHeld(InputCode::Key::A))
				{
					m_Velocity -= transform.GetRightDirection() * m_CameraSpeed;
				}

				if (Input::Get().GetKeyHeld(InputCode::Key::D))
				{
					m_Velocity += transform.GetRightDirection() * m_CameraSpeed;
				}

				if (Input::Get().GetKeyHeld(InputCode::Key::Q))
				{
					m_Velocity -= transform.GetUpDirection() * m_CameraSpeed;
				}

				if (Input::Get().GetKeyHeld(InputCode::Key::E))
				{
					m_Velocity += transform.GetUpDirection() * m_CameraSpeed;
				}
			}

			if (m_Velocity.length() > Maths::M_EPSILON)
			{
				glm::vec3 position = transform.GetLocalPosition();
				position += m_Velocity * dt;
				transform.SetLocalPosition(position);
				m_Velocity = m_Velocity * pow(m_DampeningFactor, dt);
			}
		}
	}

	void EditorCameraController::UpdateScroll(Maths::Transform& transform, float offset, float dt)
	{
		if (m_2DMode)
		{
			float multiplier = 2.0f;
			if (Input::Get().GetKeyHeld(InputCode::Key::LeftShift))
			{
				multiplier = 10.0f;
			}

			if (offset != 0.0f)
			{
				m_ZoomVelocity += dt * offset * multiplier;
			}

			if (!Maths::Equals(m_ZoomVelocity, 0.0f))
			{
				//float scale = 1.0f; // camera->GetScale();

				float scale = m_CurrentCamera->GetScale();

				scale -= m_ZoomVelocity;

				if (scale < 0.15f)
				{
					scale = 0.15f;
					m_ZoomVelocity = 0.0f;
				}
				else
				{
					m_ZoomVelocity = m_ZoomVelocity * pow(m_ZoomDampeningFactor, dt);
				}

				m_CurrentCamera->SetScale(scale);
				// camera->SetScale(scale);
			}
		}
		else
		{

			if (offset != 0.0f)
			{
				m_ZoomVelocity -= dt * offset * 10.0f;
			}

			if (!Maths::Equals(m_ZoomVelocity, 0.0f))
			{
				glm::vec3 pos = transform.GetLocalPosition();
				pos += transform.GetForwardDirection() * m_ZoomVelocity;
				m_ZoomVelocity = m_ZoomVelocity * pow(m_ZoomDampeningFactor, dt);

				transform.SetLocalPosition(pos);
			}
		}
	}

	void EditorCameraController::StopMovement()
	{
		m_ZoomVelocity = 0.0f;
		m_Velocity = glm::vec3(0.0f);
		m_RotateVelocity = glm::vec2(0.0f);
	}
}