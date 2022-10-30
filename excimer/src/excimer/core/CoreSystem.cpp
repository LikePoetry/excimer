#include "hzpch.h"
#include "CoreSystem.h"
#include "ExLog.h"
#include "Version.h"
#include "JobSystem.h"
#include "VFS.h"

bool Excimer::Internal::CoreSystem::Init(int argc, char** argv)
{
	Excimer::Debug::Log::OnInit();
	EXCIMER_LOG_INFO("Excimer - Version {0}.{1}.{2}", Excimer::ExcimerVersion.major, Excimer::ExcimerVersion.minor, Excimer::ExcimerVersion.patch);

	System::JobSystem::OnInit();
	EXCIMER_LOG_INFO("Initialising Job System");

	VFS::Get();
	EXCIMER_LOG_INFO("Initialising VFS System");


	return true;
}

void Excimer::Internal::CoreSystem::Shutdown()
{
	EXCIMER_LOG_INFO("Shutting down system ...");

	EXCIMER_LOG_INFO("release VFS system ...");
	VFS::Release();

	EXCIMER_LOG_INFO("release log system ...");
	Excimer::Debug::Log::OnRelease();
}
