#pragma once

namespace Excimer
{
	namespace Maths
	{
        /// Return the smaller of two values.
        template <class T, class U>
        inline T Min(T lhs, U rhs)
        {
            return lhs < rhs ? lhs : rhs;
        }

        /// Return the larger of two values.
        template <class T, class U>
        inline T Max(T lhs, U rhs)
        {
            return lhs > rhs ? lhs : rhs;
        }
	}
}