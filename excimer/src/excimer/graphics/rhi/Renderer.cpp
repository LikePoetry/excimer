#include "hzpch.h"
#include "Renderer.h"

namespace Excimer
{
	namespace Graphics
	{
		Renderer* (*Renderer::CreateFunc)() = nullptr;

		Renderer* Renderer::s_Instance = nullptr;
	}
}