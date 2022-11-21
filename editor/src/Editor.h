#pragma once
#include <excimer/core/Application.h>

#include <imgui/imgui.h>
namespace Excimer
{
	class Editor : public Application
	{
		friend class Application;

	public:
		Editor();
		virtual ~Editor();

		void OnEvent(Event& e) override;
		void OnImGui() override;

		void Init() override;


		void DrawMenuBar();
	};
}