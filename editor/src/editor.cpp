#include "Editor.h"
#include "excimer/core/os/Window.h"
#include "excimer/core/Profiler.h"

namespace Excimer
{
	Editor::Editor() :Application()
	{

	}

	Editor::~Editor()
	{
	}

	void Editor::Init()
	{
		Application::Init();

		Application::Get().GetWindow()->SetEventCallback(BIND_EVENT_FN(Editor::OnEvent));
	}

	void Editor::OnEvent(Event& e)
	{
		EXCIMER_PROFILE_FUNCTION();
		// TODO

		Application::OnEvent(e);
	}
}