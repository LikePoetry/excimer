#pragma once
#include "excimer/core/ExLog.h"
#include "excimer/core/Core.h"

#include <string>


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
        static Window* Create(const WindowDesc& windowDesc);

        bool Initialise(const WindowDesc& windowDesc);

        bool HasInitialised() const
        {
            return m_Init;
        };

        virtual ~Window();

        virtual void SetBorderlessWindow(bool borderless) = 0;

        virtual void SetIcon(const std::string& filePath, const std::string& smallIconFilePath = "") = 0;
    protected:
        static Window* (*CreateFunc)(const WindowDesc&);
        Window() = default;

        bool m_Init = false;
        bool m_VSync = false;
        bool m_HasResized = false;
        bool m_WindowFocus = true;
    };
}