#pragma once
#include "excimer/core/Core.h"

namespace Excimer 
{
	class EXCIMER_EXPORT Application
	{
		friend class Editor;

	public:
		Application();

		virtual void Init();
	};

	// �ڿͻ����ж���
	Application* CreateApplication();
}