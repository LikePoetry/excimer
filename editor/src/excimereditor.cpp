#include "excimerengine.h"
#include "excimer/core/EntryPoint.h"
#include "excimer/core/Profiler.h"
#include "Editor.h"


Excimer::Application* Excimer::CreateApplication()
{
	EXCIMER_LOG_INFO("Start Clint!");
	return new Editor();
}