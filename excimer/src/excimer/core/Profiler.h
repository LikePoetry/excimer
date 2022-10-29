#pragma once
#define TRACY_CALLSTACK 1
#include <Tracy/Tracy.hpp>

#define EXCIMER_PROFILE_SCOPE(name) ZoneScopedN(name)
#define EXCIMER_PROFILE_FUNCTION() ZoneScoped
#define EXCIMER_PROFILE_FRAMEMARKER() FrameMark
#define EXCIMER_PROFILE_LOCK(type, var, name) TracyLockableN(type, var, name)
#define EXCIMER_PROFILE_LOCKMARKER(var) LockMark(var)
#define EXCIMER_PROFILE_SETTHREADNAME(name) tracy::SetThreadName(name)