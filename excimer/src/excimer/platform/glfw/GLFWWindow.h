#pragma once
#include "excimer/core/os/Window.h"
#include "excimer/core/Profiler.h"
#include "excimer/core/ExLog.h"

struct GLFWwindow;

namespace Excimer
{
	namespace Graphics
	{
		enum class RenderAPI :uint32_t;
	}

	class EXCIMER_EXPORT GLFWWindow :public Window
	{
	public:
		GLFWWindow(const WindowDesc& properties);
		~GLFWWindow();

		void SetBorderlessWindow(bool borderless) override;
		void HideMouse(bool hide) override;
		void SetMousePosition(const glm::vec2& pos) override;
		virtual void ProcessInput() override;

		bool Init(const WindowDesc& properties);

		inline void* GetHandle() override
		{
			return m_Handle;
		}

		inline void SetEventCallback(const EventCallbackFn& callback) override
		{
			m_Data.EventCallback = callback;
		}

		inline uint32_t GetWidth() const override
		{
			return m_Data.Width;
		}
		inline uint32_t GetHeight() const override
		{
			return m_Data.Height;
		}

		inline float GetDPIScale() const override
		{
			return m_Data.DPIScale;
		}

		inline float GetScreenRatio() const override
		{
			return (float)m_Data.Width / (float)m_Data.Height;
		}

		void SetIcon(const std::string& file, const std::string& smallIconFilePath = "") override;
		static void MakeDefault();

	protected:
		static Window* CreateFuncGLFW(const WindowDesc& properties);

		GLFWwindow* m_Handle;

		struct WindowData
		{
			std::string Title;
			uint32_t Width, Height;
			bool VSync;
			bool Exit;
			Graphics::RenderAPI m_RenderAPI;
			float DPIScale;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}