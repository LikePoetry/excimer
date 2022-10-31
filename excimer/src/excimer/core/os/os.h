#pragma once
#include "excimer/core/Core.h"

#include <string>

namespace Excimer
{
	enum PowerState
	{
        POWERSTATE_UNKNOWN,
        POWERSTATE_ON_BATTERY,
        POWERSTATE_NO_BATTERY,
        POWERSTATE_CHARGING,
        POWERSTATE_CHARGED
	};

    class EXCIMER_EXPORT OS
    {
    public:
        OS() = default;
        virtual ~OS() = default;

        virtual void Run() = 0;

        static void Create();
        static void Release();
        static void SetInstance(OS* instance)
        {
            s_Instance = instance;
        }

        static OS* Instance()
        {
            return s_Instance;
        }

        virtual std::string GetExecutablePath() = 0;

    protected:
        static OS* s_Instance;

    };

    
}