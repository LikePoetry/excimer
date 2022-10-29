#pragma once
#include <stdint.h>

#define EXCIMER_EXPORT

#define EXCIMER_BREAK() __debugbreak();

#define EXCIMER_ASSERT_MESSAGE(condition, ...)     \
    {                                            \
        if(!(condition))                         \
        {                                        \
            EXCIMER_LOG_ERROR("Assertion Failed"); \
            EXCIMER_LOG_ERROR(__VA_ARGS__);        \
            EXCIMER_BREAK();                       \
        }                                        \
    }

#define EXCIMER_CORE_ASSERT EXCIMER_ASSERT_MESSAGE

#define EXCIMER_ASSERT EXCIMER_CORE_ASSERT