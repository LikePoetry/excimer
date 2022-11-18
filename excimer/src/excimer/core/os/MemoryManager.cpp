#include "hzpch.h"
#include "MemoryManager.h"
#include "excimer/core/StringUtilities.h"
#include "excimer/core/ExLog.h"

namespace Excimer
{
    MemoryManager* MemoryManager::s_Instance = nullptr;

    MemoryManager::MemoryManager()
    {
    }

    void MemoryManager::OnInit()
    {
    }

    void MemoryManager::OnShutdown()
    {
        if (s_Instance)
            delete s_Instance;
    }

    MemoryManager* MemoryManager::Get()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new MemoryManager();
        }
        return s_Instance;
    }

    void SystemMemoryInfo::Log()
    {
        std::string apm, tpm, avm, tvm;

        apm = StringUtilities::BytesToString(availablePhysicalMemory);
        tpm = StringUtilities::BytesToString(totalPhysicalMemory);
        avm = StringUtilities::BytesToString(availableVirtualMemory);
        tvm = StringUtilities::BytesToString(totalVirtualMemory);

        EXCIMER_LOG_INFO("Memory Info:");
        EXCIMER_LOG_INFO("\tPhysical Memory : {0} / {1}", apm, tpm);
        EXCIMER_LOG_INFO("\tVirtual Memory : {0} / {1}: ", avm, tvm);
    }
}
