#include"hzpch.h"
#include "ExLog.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Excimer::Debug::Log::s_CoreLogger;
std::vector<spdlog::sink_ptr> sinks;

void Excimer::Debug::Log::OnInit()
{
	sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());// Debug
	 // sinks.emplace_back(std::make_shared<ImGuiConsoleSink_mt>()); // ImGuiConsole

#ifdef LOG_TO_TEXT_FILE
	auto logFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("ExcimerLog.txt", 1048576 * 5, 3);
	sinks.emplace_back(logFileSink); // Log file
#endif

	// create the loggers;
	s_CoreLogger = std::make_shared<spdlog::logger>("Excimer", begin(sinks), end(sinks));
	spdlog::register_logger(s_CoreLogger);

	// configure the loggers
	spdlog::set_pattern("%^[%T] %v%$");
	s_CoreLogger->set_level(spdlog::level::trace);
}

void Excimer::Debug::Log::OnRelease()
{
	s_CoreLogger.reset();
	spdlog::shutdown();
}

void Excimer::Debug::Log::AddSink(spdlog::sink_ptr& sink)
{
	s_CoreLogger->sinks().push_back(sink);
	s_CoreLogger->set_pattern("%^[%T] %v%$");
}
