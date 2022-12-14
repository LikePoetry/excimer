#pragma once
#include "excimer/core/ExLog.h"
#include "excimer/core/Core.h"
#include "excimer/events/Event.h"
#include "excimer/core/Reference.h"
#include "excimer/graphics/rhi/GraphicsContext.h"
#include "excimer/graphics/rhi/SwapChain.h"

#include <string>

#include <glm/vec2.hpp>
#include <glm/fwd.hpp>


namespace Excimer
{
	struct EXCIMER_EXPORT WindowDesc
	{
		WindowDesc(uint32_t width = 1280, uint32_t height = 720, int renderAPI = 0, const std::string& title = "Slight", bool fullscreen = false, bool vSync = true, bool borderless = false, const std::string& filepath = "")
			: Width(width)
			, Height(height)
			, Title(title)
			, Fullscreen(fullscreen)
			, VSync(vSync)
			, Borderless(borderless)
			, RenderAPI(renderAPI)
			, FilePath(filepath)
		{
		}

		uint32_t Width, Height;
		bool Fullscreen;
		bool VSync;
		bool Borderless;
		bool ShowConsole = true;
		std::string Title;
		int RenderAPI;
		std::string FilePath;
	};

	class EXCIMER_EXPORT Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static Window* Create(const WindowDesc& windowDesc);

		bool Initialise(const WindowDesc& windowDesc);

		bool HasInitialised() const
		{
			return m_Init;
		};

		virtual ~Window();

		virtual void ProcessInput() {};
		virtual void SetBorderlessWindow(bool borderless) = 0;
		virtual void SetWindowTitle(const std::string& title) = 0;
		virtual void OnUpdate() = 0;

		virtual void* GetHandle()
		{
			return nullptr;
		};

		virtual void SetIcon(const std::string& filePath, const std::string& smallIconFilePath = "") = 0;
		virtual void Maximise() {};
		virtual float GetScreenRatio() const = 0;
		virtual void HideMouse(bool hide) {};
		virtual void SetMousePosition(const glm::vec2& pos) {};
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void UpdateCursorImGui() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual float GetDPIScale() const { return 1.0f; }

		void SetWindowFocus(bool focus) { m_WindowFocus = focus; }
		bool GetWindowFocus() const { return m_WindowFocus; }

		const SharedPtr<Excimer::Graphics::SwapChain>& GetSwapChain() const { return m_SwapChain; }
		const SharedPtr<Excimer::Graphics::GraphicsContext>& GetGraphicsContext() const { return m_GraphicsContext; }

	protected:
		static Window* (*CreateFunc)(const WindowDesc&);
		Window() = default;

		bool m_Init = false;
		bool m_VSync = false;
		bool m_HasResized = false;
		bool m_WindowFocus = true;

		SharedPtr<Excimer::Graphics::SwapChain> m_SwapChain;
		SharedPtr<Excimer::Graphics::GraphicsContext> m_GraphicsContext;
	};
}