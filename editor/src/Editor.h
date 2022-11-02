#pragma once
#include <excimer/core/Application.h>

namespace Excimer
{
	class Editor : public Application
	{
		friend class Application;

	public:
		Editor();
		virtual ~Editor();

		void Init() override;
	};
}