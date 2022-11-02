#pragma once
#include "CoreSystem.h"
#include "excimer/platform/windows/WindowsOS.h"
#include <iostream>

extern Excimer::Application* Excimer::CreateApplication();

int main()
{
	std::cout << "Hello Excimer!" << std::endl;
	if (!Excimer::Internal::CoreSystem::Init(0,nullptr))
		return 0;

	auto windowsOS = new Excimer::WindowsOS();
	Excimer::OS::SetInstance(windowsOS);

	windowsOS->Init();

	Excimer::CreateApplication();

	windowsOS->Run();
	delete windowsOS;

	Excimer::Internal::CoreSystem::Shutdown();
	return 0;
}