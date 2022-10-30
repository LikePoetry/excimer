#pragma once
#include "CoreSystem.h"

#include <iostream>

extern Excimer::Application* Excimer::CreateApplication();

int main()
{
	std::cout << "Hello Excimer!" << std::endl;
	if (!Excimer::Internal::CoreSystem::Init(0,nullptr))
		return 0;



	return 0;
}