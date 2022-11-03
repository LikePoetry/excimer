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

		void OnEvent(Event& e) override;

		void Init() override;
	};
}