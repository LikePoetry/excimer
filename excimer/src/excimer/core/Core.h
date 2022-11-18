#pragma once
#include <stdint.h>

#define MEM_ALIGNMENT 16
#define MEM_ALIGN __declspec(align(MEM_ALIGNMENT))

#define EXCIMER_EXPORT

#define EXCIMER_HIDDEN

#define MAX_OBJECTS 2048

#define BIT(x) (1 << x)

#define EXCIMER_BREAK() __debugbreak();

#define EXCIMER_ASSERT_NO_MESSAGE(condition)        \
    {                                               \
        if(!(condition))                            \
        {                                           \
            EXCIMER_LOG_ERROR("Assertion Failed!"); \
            EXCIMER_BREAK();                        \
        }                                           \
    }

#define EXCIMER_ASSERT_MESSAGE(condition, ...)      \
    {                                               \
        if(!(condition))                            \
        {                                           \
            EXCIMER_LOG_ERROR("Assertion Failed");  \
            EXCIMER_LOG_ERROR(__VA_ARGS__);         \
            EXCIMER_BREAK();                        \
        }                                           \
    }

#define EXCIMER_CORE_ASSERT EXCIMER_ASSERT_MESSAGE

#define EXCIMER_ASSERT EXCIMER_CORE_ASSERT


#define NONCOPYABLE(type_identifier)                             \
    type_identifier(const type_identifier&)            = delete; \
    type_identifier& operator=(const type_identifier&) = delete;


#define DISABLE_WARNING_PUSH __pragma(warning(push))
#define DISABLE_WARNING_POP __pragma(warning(pop))
#define DISABLE_WARNING(warningNumber) __pragma(warning(disable \
                                                        : warningNumber))

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER DISABLE_WARNING(4100)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION DISABLE_WARNING(4505)
#define DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE DISABLE_WARNING(4267)