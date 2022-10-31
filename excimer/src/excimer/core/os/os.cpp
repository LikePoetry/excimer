#include "hzpch.h"
#include "excimer/core/ExLog.h"
#include "os.h"

#include "excimer/platform/windows/WindowsOS.h"

namespace Excimer
{
	OS* OS::s_Instance = nullptr;

	void OS::Create()
	{
		EXCIMER_ASSERT(!s_Instance, "OS already exists!");

		s_Instance = new WindowsOS();
	}

	void OS::Release()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}
}