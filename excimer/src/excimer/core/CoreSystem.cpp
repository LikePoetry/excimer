#include "hzpch.h"
#include "CoreSystem.h"
#include "ExLog.h"
#include "Version.h"

bool Excimer::Internal::CoreSystem::Init(int argc, char** argv)
{
	Excimer::Debug::Log::OnInit();
	EXCIMER_LOG_INFO("Excimer - Version {0}.{1}.{2}", Excimer::ExcimerVersion.major, Excimer::ExcimerVersion.minor, Excimer::ExcimerVersion.patch);


	return false;
}

void Excimer::Internal::CoreSystem::Shutdown()
{
	EXCIMER_LOG_INFO("Shutting down system ...");

	EXCIMER_LOG_INFO("release log system ...");
	Excimer::Debug::Log::OnRelease();
}
