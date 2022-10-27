#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>



// Core log macros
#define EXCIMER_LOG_TRACE(...)		SPDLOG_LOGGER_CALL(::Excimer::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::trace, __VA_ARGS__)
#define EXCIMER_LOG_INFO(...)		SPDLOG_LOGGER_CALL(::Excimer::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::info, __VA_ARGS__)
#define EXCIMER_LOG_WARN(...)		SPDLOG_LOGGER_CALL(::Excimer::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::warn, __VA_ARGS__)
#define EXCIMER_LOG_ERROR(...)		SPDLOG_LOGGER_CALL(::Excimer::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::err, __VA_ARGS__)
#define EXCIMER_LOG_CRITICAL(...)	SPDLOG_LOGGER_CALL(::Excimer::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::critical, __VA_ARGS__)

namespace Excimer 
{
	namespace Debug
	{
		class EXCIMER_EXPORT Log
		{
		public:
			static void OnInit();
			static void OnRelease();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			
			static void AddSink(spdlog::sink_ptr& sink);

		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
		};
	}
}