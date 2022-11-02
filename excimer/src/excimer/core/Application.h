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

	// 在客户端中定义
	Application* CreateApplication();
}