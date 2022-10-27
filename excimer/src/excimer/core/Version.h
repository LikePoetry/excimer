#pragma once
namespace Excimer
{
	struct Version
	{
		int major = 0;
		int minor = 0;
		int patch = 1;

	};

	constexpr Version const ExcimerVersion = Version();
}