#pragma once
#include "excimer/core/os/os.h"

namespace Excimer
{
	class EXCIMER_EXPORT WindowsOS:public OS
	{
    public:
        WindowsOS() = default;
        ~WindowsOS() = default;

        void Init();
        void Run() override;
        std::string GetExecutablePath() override;
	};
}