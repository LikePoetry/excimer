#pragma once
#include <cstddef>

#include "excimer/core/Profiler.h"

namespace Excimer
{
	inline void HashCombine(std::size_t& seed){}

	template <typename T,typename... Rest>
    inline void HashCombine(std::size_t& seed, const T& v, Rest... rest)
    {
        EXCIMER_PROFILE_FUNCTION();
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        HashCombine(seed, rest...);
    }
}